#include "winsimple.hpp"
#include "winsimple-compute.hpp"
#include <chrono>
#include <vector>
#include <cmath>

int main() {
    // 1. Setup window and screen capture
    ws::Screen screen;
    ws::Window window(1920, 1080, "Wavy Effect - Press ESC to exit");
    SetWindowDisplayAffinity(window.hwnd, WDA_EXCLUDEFROMCAPTURE);
    window.setVisible(true);

    ws::Vec2i screenSize = screen.getSize();
    int w = screenSize.x;
    int h = screenSize.y;
    int totalPixels = w * h;

    // 2. Initialise compute shader wrapper
    ws::Compute compute;
    if (!compute.Init()) {
        MessageBoxA(nullptr, "D3D11 init failed", "Error", MB_OK);
        return -1;
    }

    // 3. Load compute shader (2D wave effect on a flat buffer)
    //    The shader reads input buffer, writes output buffer.
    //    We pass width, height, and time via a constant buffer.
    const char* shaderSource = R"(
        struct Pixel { uint color; };  // ARGB packed into uint
        RWStructuredBuffer<Pixel> input : register(u0);
        RWStructuredBuffer<Pixel> output : register(u1);
        cbuffer Params : register(b0) {
            uint width;
            uint height;
            float time;
        };

        [numthreads(64, 1, 1)]
        void CSMain(uint3 id : SV_DispatchThreadID) {
            uint idx = id.x;
            if (idx >= width * height) return;

            uint x = idx % width;
            uint y = idx / width;

            // Normalised coordinates (0..1)
            float u = (float)x / width;
            float v = (float)y / height;

            // Sine wave shift (horizontal only)
            float offsetX = sin(v * 6.28318f + time) * 0.05f;  // amplitude 5%
            float u_src = u + offsetX;
            if (u_src < 0.0f || u_src >= 1.0f) {
                // Out of bounds → black
                output[idx].color = 0xFF000000;
                return;
            }
            int srcX = (int)(u_src * width);
            int srcIdx = srcX + y * width;
            output[idx].color = input[srcIdx].color;
        }
    )";

    if (!compute.loadShader(shaderSource, "CSMain")) {
        MessageBoxA(nullptr, "Shader compile failed", "Error", MB_OK);
        return -1;
    }

    // 4. Create constant buffer for parameters (width, height, time)
    struct Params {
        uint32_t width;
        uint32_t height;
        float time;
    };
    Params params = { (uint32_t)w, (uint32_t)h, 0.0f };

    // We need to create a D3D11 constant buffer and set it.
    // Since ws::Compute does not expose constant buffer methods,
    // we use getDevice() and getContext() to create and bind it manually.
    ID3D11Device* device = compute.getDevice();
    ID3D11DeviceContext* ctx = compute.getContext();

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(Params);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ID3D11Buffer* cbuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, &cbuffer);
    if (FAILED(hr)) {
        MessageBoxA(nullptr, "Failed to create constant buffer", "Error", MB_OK);
        return -1;
    }

    // 5. Prepare a buffer for image data (ARGB packed into uint)
    //    We'll use std::vector<uint32_t> but setData expects vector<float>.
    //    Workaround: store as uint and reinterpret cast.
    std::vector<uint32_t> imageData(totalPixels);
    std::vector<uint32_t> outputData(totalPixels);

    // 6. Create two structured buffers (input and output) via setData.
    //    We'll upload input buffer, run shader, then download from output buffer.
    //    However, setData only manages one internal buffer. We need two.
    //    The current ws::Compute only has a single m_buffer / m_uav.
    //    To have two buffers, we must create the second buffer manually.
    //    But we can also do everything with one buffer by reading and writing
    //    to the same buffer – but careful with race conditions.
    //    Simpler: let's create two separate structured buffers using D3D11 directly,
    //    and then use runShader (which uses the internal UAV). Actually runShader
    //    uses the internal m_uav only. So we can't easily swap.
    //
    //    Given the limitations, it's better to write a shader that does in‑place
    //    transformation (reads and writes same buffer). For wave effect we need
    //    source and destination separate to avoid reading overwritten pixels.
    //    But if we process pixels in order, and the offset is only horizontal,
    //    we could do in‑place if we process from left to right? Not safe.
    //
    //    Because your ws::Compute only supports one UAV at a time, the cleanest
    //    is to extend it with a second buffer or use a temporary texture.
    //    I'll instead show you how to do it with two buffers by using the raw
    //    D3D11 calls (which you already have access to via getDevice/Context).
    //    This keeps the example working without modifying your class.

    // 7. Create two structured buffers (input and output) using D3D11 directly.
    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.ByteWidth = totalPixels * sizeof(uint32_t);
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bufDesc.StructureByteStride = sizeof(uint32_t);
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    ID3D11Buffer* inputBuffer = nullptr;
    ID3D11Buffer* outputBuffer = nullptr;
    device->CreateBuffer(&bufDesc, nullptr, &inputBuffer);
    device->CreateBuffer(&bufDesc, nullptr, &outputBuffer);

    // Create UAVs for both
    ID3D11UnorderedAccessView* inputUAV = nullptr;
    ID3D11UnorderedAccessView* outputUAV = nullptr;
    device->CreateUnorderedAccessView(inputBuffer, nullptr, &inputUAV);
    device->CreateUnorderedAccessView(outputBuffer, nullptr, &outputUAV);

    // We also need a SRV for input? Not needed because UAV can be read in shader.
    // But the shader uses RWStructuredBuffer for input – that's fine, it reads via UAV.

    // 8. Main loop
    auto startTime = std::chrono::steady_clock::now();
    while (window.isOpen()) {
        // Capture screen snapshot
        ws::Texture snapshot = screen.getSnapshot();

        // Convert snapshot to packed ARGB in inputData (vector<uint32_t>)
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                ws::Hue hue = snapshot.getPixel(x, y);
                uint32_t color = (hue.a << 24) | (hue.r << 16) | (hue.g << 8) | hue.b;
                imageData[y * w + x] = color;
            }
        }

        // Update input buffer
        ctx->UpdateSubresource(inputBuffer, 0, nullptr, imageData.data(), 0, 0);

        // Update constant buffer with current time
        auto now = std::chrono::steady_clock::now();
        params.time = std::chrono::duration<float>(now - startTime).count();
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(ctx->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            memcpy(mapped.pData, &params, sizeof(params));
            ctx->Unmap(cbuffer, 0);
        }

        // Bind shader and resources
        ctx->CSSetConstantBuffers(0, 1, &cbuffer);
        ctx->CSSetUnorderedAccessViews(0, 1, &inputUAV, nullptr);
        ctx->CSSetUnorderedAccessViews(1, 1, &outputUAV, nullptr);

		compute.runShader(groups, 1, 1);

        // Unbind
		ID3D11UnorderedAccessView* nullUAV = nullptr;
		ctx->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		ctx->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);

        // Read back output buffer
        // Create staging buffer for reading
        D3D11_BUFFER_DESC stagingDesc = {};
        stagingDesc.ByteWidth = totalPixels * sizeof(uint32_t);
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        ID3D11Buffer* stagingBuffer = nullptr;
        device->CreateBuffer(&stagingDesc, nullptr, &stagingBuffer);
        ctx->CopyResource(stagingBuffer, outputBuffer);
        D3D11_MAPPED_SUBRESOURCE mapOut;
        if (SUCCEEDED(ctx->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mapOut))) {
            memcpy(outputData.data(), mapOut.pData, totalPixels * sizeof(uint32_t));
            ctx->Unmap(stagingBuffer, 0);
        }
        stagingBuffer->Release();

        // Convert outputData back to a ws::Texture
        ws::Texture resultTex;
        resultTex.create(w, h, ws::Hue::transparent);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                uint32_t col = outputData[y * w + x];
                ws::Hue hue((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF, (col >> 24) & 0xFF);
                resultTex.setPixel(x, y, hue);
            }
        }

        // Draw the result
        window.clear();
        ws::Sprite sprite(resultTex);
        window.draw(sprite);
        window.display();

        // Exit on ESC
        if (ws::Global::getButton(VK_ESCAPE))
            break;
    }

    // Cleanup
    inputBuffer->Release();
    outputBuffer->Release();
    inputUAV->Release();
    outputUAV->Release();
    cbuffer->Release();

    return 0;
}