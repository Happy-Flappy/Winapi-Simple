#ifndef WINSIMPLE_COMPUTE
#define WINSIMPLE_COMPUTE


#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include <cstdint>

#ifdef _MSC_VER
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif


namespace ws
{
	class Compute
	{
		public:
		Compute() = default;
		~Compute(){cleanup();}
		
		
		
		bool Init()
		{
			cleanup();
			HRESULT hr = D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);
			return SUCCEEDED(hr);
		}
		
		bool loadShader(const std::string& shaderStr, const char* entryPoint = "CSMain")
		{
			if(!device) return false;
			
			ID3DBlob* shaderBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;
			
			HRESULT hr = D3DCompile(shaderStr.c_str(),shaderStr.size(),nullptr,nullptr,nullptr,entryPoint,"cs_5_0",D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&shaderBlob,&errorBlob);
			
			if(FAILED(hr))
			{
				if(errorBlob)
				{
					OutputDebugStringA((char*)errorBlob->GetBufferPointer());
					errorBlob->Release();					
				}
				return false;
			}
			
			hr = device->CreateComputeShader(shaderBlob->GetBufferPointer(),shaderBlob->GetBufferSize(),nullptr,&computeShader);
			
			shaderBlob->Release();
			
			return SUCCEEDED(hr);
			
		}


		
		
		void runShader(int x, int y, int z) const 
		{
			if (!context || !computeShader)
				return;

			context->CSSetShader(computeShader, nullptr, 0);
		
			context->Dispatch(static_cast<UINT>(x), static_cast<UINT>(y), static_cast<UINT>(z));
		
			context->CSSetShader(nullptr, nullptr, 0);			
		}		
		
		
		bool setData(const std::vector<float>& data)
		{
			if(!device || data.empty()) return false;
			
			m_count = (int)data.size();

			if (m_buffer) m_buffer->Release();
			if (m_uav) m_uav->Release();

			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = m_count * sizeof(float);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.StructureByteStride = sizeof(float);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			D3D11_SUBRESOURCE_DATA init = { data.data() };
			HRESULT hr = device->CreateBuffer(&desc, &init, &m_buffer);
			if (FAILED(hr)) return false;

			hr = device->CreateUnorderedAccessView(m_buffer, nullptr, &m_uav);
			if (FAILED(hr)) {
				m_buffer->Release();
				m_buffer = nullptr;
				return false;
			}
			return true;			
		}
		
		bool getData(std::vector<float>& output) const 
		{
			if (!device || !context || !m_buffer || m_count == 0) 
				return false;
			output.resize(m_count);


			D3D11_BUFFER_DESC stagingDesc = {};
			stagingDesc.ByteWidth = m_count * sizeof(float);
			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			ID3D11Buffer* staging = nullptr;
			HRESULT hr = device->CreateBuffer(&stagingDesc, nullptr, &staging);
			if (FAILED(hr)) return false;

			context->CopyResource(staging, m_buffer);

			D3D11_MAPPED_SUBRESOURCE mapped;
			hr = context->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);
			if (SUCCEEDED(hr)) {
				memcpy(output.data(), mapped.pData, m_count * sizeof(float));
				context->Unmap(staging, 0);
			}
			staging->Release();
			return SUCCEEDED(hr);
		}		
		
		
		ID3D11UnorderedAccessView* getUAV() const { return m_uav; }
		ID3D11Device* getDevice() const { return device; }
		ID3D11DeviceContext* getContext() const { return context; }		
		
		
		private:
		
		void cleanup()
		{
			if (m_uav) m_uav->Release();
			if (m_buffer) m_buffer->Release();
			if (computeShader) computeShader->Release();
			if (context) context->Release();
			if (device) device->Release();
			m_uav = nullptr;
			m_buffer = nullptr;
			computeShader = nullptr;
			context = nullptr;
			device = nullptr;
			m_count = 0;
		}

		ID3D11Device*       device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		ID3D11ComputeShader* computeShader = nullptr;
		ID3D11Buffer* m_buffer = nullptr;
		ID3D11UnorderedAccessView* m_uav = nullptr;
		int m_count = 0;		
	};
};


#endif