#include "winsimple-shaders.hpp"


int main() {
    
	ws::Shader shader;
	
    const char* hlsl = R"(
        RWStructuredBuffer<float> outputBuffer : register(u0);
        [numthreads(64, 1, 1)]
        void CSMain(uint3 id : SV_DispatchThreadID) {
            outputBuffer[id.x] = outputBuffer[id.x] * 2.0f;
        }
    )";

	shader.loadFromFile("shader.hlsl");
	
	
    // 4. Set up a data buffer (UAV) to process
    float data[128];
    for (int i = 0; i < 128; i++) data[i] = (float)i; // Fill with 0..127

    // Create a buffer description for a read/write structured buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(float) * 128;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UAV flag
    bufferDesc.StructureByteStride = sizeof(float);
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    ID3D11Buffer* buffer = nullptr;
    HRESULT hr = ws::loadDX.device->CreateBuffer(&bufferDesc, &initData, &buffer);
    if(FAILED(hr)) { printf("Failed to create buffer.\n"); return 1; }

    // Create the UAV (Unordered Access View) to bind to the shader
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = 128;

    ID3D11UnorderedAccessView* uav = nullptr;
    hr = ws::loadDX.device->CreateUnorderedAccessView(buffer, &uavDesc, &uav);
    if (FAILED(hr)) { printf("Failed to create UAV.\n"); return 1; }

    // 5. Run the GPU Compute Pipeline!
    ws::loadDX.context->CSSetShader(ws::loadDX.computeShader, nullptr, 0);
    ws::loadDX.context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

    // Dispatch: Total threads = 128. Since our group has 64 threads, we need 2 groups.
    ws::loadDX.context->Dispatch(2, 1, 1); // <-- This is the "Draw" equivalent for compute!

    // 6. Read the data back from GPU to CPU
    D3D11_BUFFER_DESC readbackDesc = {};
    readbackDesc.ByteWidth = sizeof(float) * 128;
    readbackDesc.Usage = D3D11_USAGE_STAGING;
    readbackDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    readbackDesc.BindFlags = 0;

    ID3D11Buffer* readbackBuffer = nullptr;
    ws::loadDX.device->CreateBuffer(&readbackDesc, nullptr, &readbackBuffer);

    ws::loadDX.context->CopyResource(readbackBuffer, buffer);

    D3D11_MAPPED_SUBRESOURCE mapped;
    ws::loadDX.context->Map(readbackBuffer, 0, D3D11_MAP_READ, 0, &mapped);
    float* result = (float*)mapped.pData;

    printf("Results:\n");
    for(int a=0; a < 10; a++) 
		std::cerr << result[a];
	
    ws::loadDX.context->Unmap(readbackBuffer, 0);

    // Cleanup
    ws::loadDX.computeShader->Release();
    buffer->Release();
    uav->Release();
    readbackBuffer->Release();
    
    system("pause");
    return 0;
}