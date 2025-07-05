#include "pch.h"
#include "dx_utils.h"

namespace Utils
{
	const wchar_t* Utils::DXUtils::TEMP_WINDOW_CLASS = L"DXUtilsTempWindow";
	bool Utils::DXUtils::s_windowClassRegistered = false;
	
	RenderAPI Utils::DXUtils::getRenderAPI()
	{
		HMODULE d3d9 = getD3D9Module();
		HMODULE d3d11 = getD3D11Module();
		HMODULE d3d12 = getD3D12Module();
	
		if (d3d9)
			return RenderAPI::DirectX9;
		if (d3d11)
			return RenderAPI::DirectX11;
		if (d3d12)
			return RenderAPI::DirectX12;
	
		return RenderAPI::Unknown;
	}
	
	HMODULE Utils::DXUtils::getD3D9Module()
	{
		return GetModuleHandleW(L"d3d9.dll");
	}
	
	HMODULE Utils::DXUtils::getD3D11Module()
	{
		return GetModuleHandleW(L"d3d11.dll");
	}
	
	HMODULE Utils::DXUtils::getD3D12Module()
	{
		return GetModuleHandleW(L"d3d12.dll");
	}
	
	HMODULE Utils::DXUtils::getDXGIModule()
	{
		return GetModuleHandleW(L"dxgi.dll");
	}
	
	bool Utils::DXUtils::createTempD3D9Device(HWND hwnd, IDirect3DDevice9** device)
	{
		HMODULE d3d9Module = getD3D9Module();
		if (!d3d9Module)
			return false;
	
		typedef IDirect3D9* (WINAPI* Direct3DCreate9_t)(UINT SDKVersion);
		auto Direct3DCreate9 = (Direct3DCreate9_t)GetProcAddress(d3d9Module, "Direct3DCreate9");
		if (!Direct3DCreate9)
			return false;
	
		IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (!d3d9)
			return false;
	
		D3DPRESENT_PARAMETERS params;
		params.BackBufferWidth = 1;
		params.BackBufferHeight = 1;
		params.BackBufferFormat = D3DFMT_UNKNOWN;
		params.BackBufferCount = 1;
		params.MultiSampleType = D3DMULTISAMPLE_NONE;
		params.MultiSampleQuality = NULL;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.hDeviceWindow = hwnd;
		params.Windowed = TRUE;
		params.EnableAutoDepthStencil = FALSE;
		params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		params.Flags = NULL;
		params.FullScreen_RefreshRateInHz = 0;
		params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
		HRESULT hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_NULLREF,
			hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
			&params,
			device);
	
		d3d9->Release();
	
		return SUCCEEDED(hr);
	}
	
	bool Utils::DXUtils::createTempD3D11Device(HWND hwnd, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapChain)
	{
		HMODULE d3d11Module = getD3D11Module();
		if (!d3d11Module)
			return false;
	
		typedef HRESULT(WINAPI* D3D11CreateDeviceAndSwapChain_t)(
			IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			const D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
			IDXGISwapChain** ppSwapChain,
			ID3D11Device** ppDevice,
			D3D_FEATURE_LEVEL* pFeatureLevel,
			ID3D11DeviceContext** ppImmediateContext);
	
		auto D3D11CreateDeviceAndSwapChain = (D3D11CreateDeviceAndSwapChain_t)GetProcAddress(d3d11Module, "D3D11CreateDeviceAndSwapChain");
		if (!D3D11CreateDeviceAndSwapChain)
			return false;
	
		DXGI_RATIONAL refreshRate;
		refreshRate.Numerator = 60;
		refreshRate.Denominator = 1;
	
		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = 800;
		bufferDesc.Height = 600;
		bufferDesc.RefreshRate = refreshRate;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = 1;
		swapChainDesc.BufferDesc.Height = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr, // Use default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr, // No software rasterizer
			0, // No flags
			featureLevels, // Feature levels to support
			ARRAYSIZE(featureLevels), // Number of feature levels
			D3D11_SDK_VERSION,
			&swapChainDesc,
			swapChain,
			device,
			&featureLevel,
			context);
	
		return SUCCEEDED(hr);
	}
	
	// Reference: https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook/blob/e65aacad5a88eb0a2f7d52e94c5cced93f2f0436/hooks.cpp
	bool Utils::DXUtils::createTempD3D12Device(HWND hwnd, ID3D12Device** device, ID3D12CommandQueue** commandQueue, IDXGISwapChain3** swapChain)
	{
		HMODULE d3d12Module = getD3D12Module();
		HMODULE dxgiModule = getDXGIModule();
		if (!d3d12Module || !dxgiModule)
			return false;
	
		typedef HRESULT(WINAPI* D3D12CreateDevice_t)(
			IUnknown* pAdapter,
			D3D_FEATURE_LEVEL MinimumFeatureLevel,
			REFIID riid,
			void** ppDevice);
		typedef HRESULT(WINAPI* CreateDXGIFactory1_t)(
			REFIID riid,
			void** ppFactory);
	
		D3D12CreateDevice_t D3D12CreateDevice = (D3D12CreateDevice_t)GetProcAddress(d3d12Module, "D3D12CreateDevice");
		CreateDXGIFactory1_t CreateDXGIFactory1 = (CreateDXGIFactory1_t)GetProcAddress(dxgiModule, "CreateDXGIFactory1");
		if (!D3D12CreateDevice || !CreateDXGIFactory1)
			return false;
	
		IDXGIFactory4* factory = nullptr;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		if (FAILED(hr))
			return false;
		
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device));
		if (FAILED(hr))
		{
			factory->Release();
			return false;
		}
		
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
	
		hr = (*device)->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue));
		if (FAILED(hr))
		{
			(*device)->Release();
			*device = nullptr;
			factory->Release();
			return false;
		}
	
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = 1;
		swapChainDesc.Height = 1;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = 0;
	
		IDXGISwapChain1* swapChain1 = nullptr;
		hr = factory->CreateSwapChainForHwnd(
			*commandQueue,
			hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1);
		if (FAILED(hr) || !swapChain1)
		{
			(*commandQueue)->Release();
			*commandQueue = nullptr;
			(*device)->Release();
			*device = nullptr;
			factory->Release();
			return false;
		}
	
		hr = swapChain1->QueryInterface(IID_PPV_ARGS(swapChain));
	
		// Always release swapChain1 after querying
		swapChain1->Release();
		factory->Release();
		
		if (FAILED(hr) || !swapChain)
		{
			(*commandQueue)->Release();
			*commandQueue = nullptr;
			(*device)->Release();
			*device = nullptr;
			return false;
		}
	
		return SUCCEEDED(hr);
	}
	
	HWND Utils::DXUtils::createTempWindow()
	{
		if (!s_windowClassRegistered)
			registerTempWindowClass();
	
		return CreateWindowW(
			TEMP_WINDOW_CLASS,
			L"Temp Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 1, 1,
			nullptr,
			nullptr,
			GetModuleHandleW(nullptr),
			nullptr);
	}
	
	void Utils::DXUtils::destroyTempWindow(HWND hwnd)
	{
		if (hwnd)
			DestroyWindow(hwnd);
	}
	
	void Utils::DXUtils::registerTempWindowClass()
	{
		WNDCLASSW wc = {};
		wc.lpfnWndProc = tempWindowProc;
		wc.hInstance = GetModuleHandleW(nullptr);
		wc.lpszClassName = TEMP_WINDOW_CLASS;
		wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		
		RegisterClassW(&wc);
		s_windowClassRegistered = true;
	}
	
	LRESULT CALLBACK Utils::DXUtils::tempWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}
	

