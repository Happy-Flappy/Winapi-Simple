#ifndef WINSIMPLE_SHADERS_HPP
#define WINSIMPLE_SHADERS_HPP

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include <d3d11.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>

#ifndef _GUIDDEF_H_
#include <guiddef.h>
#endif

#include <stdio.h>
#include <iostream>

namespace ws
{

	class LoadDX
	{
		public:
		bool loaded = false;
		
		typedef HRESULT (WINAPI* PFN_D3D11_CREATE_DEVICE)(
			IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
			const D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**,
			D3D_FEATURE_LEVEL*, ID3D11DeviceContext**
		);

		typedef HRESULT (WINAPI* PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)(
			IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
			const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*,
			IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**
		);

		typedef HRESULT (WINAPI* PFN_D3D12_CREATE_DEVICE)(
			IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**
		);

		typedef HRESULT (WINAPI* PFN_D3D_COMPILE)(
			LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*,
			LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**
		);

		typedef HRESULT (WINAPI* PFN_D3D_COMPILE_FROM_FILE)(
			LPCWSTR, const D3D_SHADER_MACRO*, ID3DInclude*,
			LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**
		);

		typedef HRESULT (WINAPI* PFN_CREATE_DXGI_FACTORY)(
			REFIID, void**
		);		

		HMODULE d3d11_dll = nullptr;
		HMODULE d3d12_dll = nullptr;
		HMODULE d3dcompiler_dll = nullptr;
		HMODULE dxgi_dll = nullptr;

		PFN_D3D11_CREATE_DEVICE              D3D11CreateDevice = nullptr;
		PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain = nullptr;
		PFN_D3D12_CREATE_DEVICE              D3D12CreateDevice = nullptr;
		PFN_D3D_COMPILE                      D3DCompile = nullptr;
		PFN_D3D_COMPILE_FROM_FILE            D3DCompileFromFile = nullptr;
		PFN_CREATE_DXGI_FACTORY              CreateDXGIFactory1 = nullptr;

        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        D3D_FEATURE_LEVEL featureLevel;
		ID3D11ComputeShader* computeShader = nullptr;

		LoadDX()
		{
			loaded = false;
			if(d3d11_dll) return;

			// Load DLLs
			d3d11_dll = LoadLibraryA("d3d11.dll");
			d3d12_dll = LoadLibraryA("d3d12.dll");
			d3dcompiler_dll = LoadLibraryA("d3dcompiler_47.dll");
			dxgi_dll = LoadLibraryA("dxgi.dll");

			if(!d3d11_dll || !d3d12_dll || !d3dcompiler_dll || !dxgi_dll)
				return;

			// Get function pointers
			D3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(d3d11_dll, "D3D11CreateDevice");
			D3D11CreateDeviceAndSwapChain = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)GetProcAddress(d3d11_dll, "D3D11CreateDeviceAndSwapChain");
			D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12_dll, "D3D12CreateDevice");
			D3DCompile = (PFN_D3D_COMPILE)GetProcAddress(d3dcompiler_dll, "D3DCompile");
			D3DCompileFromFile = (PFN_D3D_COMPILE_FROM_FILE)GetProcAddress(d3dcompiler_dll, "D3DCompileFromFile");
			CreateDXGIFactory1 = (PFN_CREATE_DXGI_FACTORY)GetProcAddress(dxgi_dll, "CreateDXGIFactory1");
			
			if(!D3D11CreateDevice || !D3D12CreateDevice || !D3DCompile || !CreateDXGIFactory1)
				return;
			
			//creating device...
			if(device) return; 
			
            if(!D3D11CreateDevice) 
			{
                printf("D3D11CreateDevice function pointer is missing.\n");
                return;
            }

            HRESULT hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                nullptr, 0, D3D11_SDK_VERSION,
                &device, &featureLevel, &context
            );

            if(FAILED(hr)) 
            {
				printf("Failed to create D3D11 device. HRESULT: 0x%08lX\n", hr);
				return;
			}
			loaded = true;
		}
		
		~LoadDX()
		{
            if(context) { context->Release(); context = nullptr; }
            if(device) { device->Release(); device = nullptr; }			
			if(d3d11_dll) { FreeLibrary(d3d11_dll); d3d11_dll = nullptr; }
			if(d3d12_dll) { FreeLibrary(d3d12_dll); d3d12_dll = nullptr; }
			if(d3dcompiler_dll) { FreeLibrary(d3dcompiler_dll); d3dcompiler_dll = nullptr; }
			if(dxgi_dll) { FreeLibrary(dxgi_dll); dxgi_dll = nullptr; }
			D3D11CreateDevice = nullptr;
			D3D12CreateDevice = nullptr;
			D3DCompile = nullptr;
			CreateDXGIFactory1 = nullptr;			
		}
	
	}loadDX;
	
	class Shader
	{
		public:
		
		//temporary
		HRESULT CreateD3D11DeviceAndSwapChain(
			IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE driverType,
			HMODULE software,
			UINT flags,
			const D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT featureLevelsCount,
			UINT sdkVersion,
			const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
			IDXGISwapChain** ppSwapChain,
			ID3D11Device** ppDevice,
			D3D_FEATURE_LEVEL* pFeatureLevel,
			ID3D11DeviceContext** ppImmediateContext
		) 
		{
			if(!loadDX.D3D11CreateDeviceAndSwapChain) return E_POINTER;
			return loadDX.D3D11CreateDeviceAndSwapChain(
				pAdapter, driverType, software, flags,
				pFeatureLevels, featureLevelsCount, sdkVersion,
				pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext
			);
		}		
		
		//temporary.
		HRESULT CreateD3D12Device(
			IUnknown* pAdapter,
			D3D_FEATURE_LEVEL minimumFeatureLevel,
			REFIID riid,
			void** ppDevice
		) 
		{
			if(!loadDX.D3D12CreateDevice) return E_POINTER;
			return loadDX.D3D12CreateDevice(pAdapter, minimumFeatureLevel, riid, ppDevice);
		}

		HRESULT CreateDXGIFactory1(REFIID riid, void** ppFactory) 
		{
			if(!loadDX.CreateDXGIFactory1) return E_POINTER;
			return loadDX.CreateDXGIFactory1(riid, ppFactory);
		}		
		
		bool loadFromMemory(std::string hlsl,std::string name = "compute")
		{
			if(!loadDX.loaded)
				return false;			
			ID3DBlob* bytecode = nullptr;
			ID3DBlob* errors = nullptr;
			
			HRESULT hr = E_POINTER;
			if(loadDX.D3DCompile)
			{
				std::string fullName = name + ".hlsl";
				hr = loadDX.D3DCompile(
				hlsl.c_str(), hlsl.size(), fullName.c_str(), nullptr, nullptr,
				"CSMain",       // entry point
				"cs_5_0",       
				0, 0,
				&bytecode, &errors);
			}
			
			if(FAILED(hr)) 
			{
				if(errors) 
				{
					printf("Compilation error: %s\n", (char*)errors->GetBufferPointer());
					errors->Release();
				}
				return false;
			}
			return addToDevice(bytecode);
		}
		
		bool loadFromFile(std::string path,std::string name = "compute")
		{
			if(!loadDX.loaded)
				return false;
			std::string fullPath = path + name + ".hlsl";
			std::wstring wFullPath(fullPath.begin(), fullPath.end());
			
			ID3DBlob* bytecode = nullptr;
			ID3DBlob* errors = nullptr;
			
			HRESULT hr = E_POINTER;
			if(loadDX.D3DCompileFromFile)
			{
				hr = loadDX.D3DCompileFromFile(
				wFullPath.c_str(),
				nullptr,nullptr,
				"CSMain","cs_5_0",       
				0, 0,
				&bytecode, &errors);
			}
			
			if(FAILED(hr)) 
			{
				if(errors) 
				{
					printf("Compilation error: %s\n", (char*)errors->GetBufferPointer());
					errors->Release();
				}
				return false;
			}
			return addToDevice(bytecode);			
		}
		
		private:
		bool addToDevice(ID3DBlob* bytecode)
		{
			if(!loadDX.loaded)
				return false;
			//note to self - this could choose the type of shader here based on whatever enum the user sets. 
			
			HRESULT hr = loadDX.device->CreateComputeShader(bytecode->GetBufferPointer(), 
			bytecode->GetBufferSize(),nullptr, &loadDX.computeShader);
			
			bytecode->Release();
			if(FAILED(hr)) 
			{ 
				printf("Failed to create compute shader.\n"); 
				return false; 
			}
			return true;			
		}
		
		
	};

}

#endif