#include "BaseApp.h"
#include "../Helper/GeneralHelper.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_baseApp->MsgProc(hwnd, msg, wParam, lParam);
}

BaseApp::BaseApp(HINSTANCE instance, int width, int height)
	:m_instance(instance),
	m_width(width),
	m_height(height),
	m_appPaused(false),
	m_appName(L"Testing"),
	m_4xMsaaQuality(0),
	m_swapChain(0),
	m_d3dDevice(0),
	m_d3dDeviceContext(0),
	m_renderTargetView(0),
	m_depthStencilBuffer(0),
	m_depthStencilState(0),
	m_depthStencilView(0),
	m_solidRS(0),
	m_wireFrameRS(0)
{
}

BaseApp::~BaseApp()
{
	//D3D Shutdown
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	ReleaseCOM(m_solidRS);
	ReleaseCOM(m_wireFrameRS);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilState);
	ReleaseCOM(m_depthStencilBuffer);
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_d3dDeviceContext);
	ReleaseCOM(m_d3dDevice);
	ReleaseCOM(m_swapChain);

	//Window Shutdown
	ChangeDisplaySettings(NULL, 0);
	DestroyWindow(m_hWnd);
	UnregisterClass(m_appName, m_instance);
	ShowCursor(true);
}

bool BaseApp::Init()
{
	if (!InitWindow())
	{
		MessageBox(0, L"Failed Creating Window", L"ERROR!", MB_OK);
		return false;
	}

	if (!InitD3D())
	{
		MessageBox(0, L"Failed Initializing DirectX", L"ERROR!", MB_OK);
		return false;
	}

	return true;
}

void BaseApp::Run()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!m_appPaused)
			{
				//do stuff
				Update(1.0f);
				Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

bool BaseApp::InitWindow()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	g_baseApp = this;

	//m_appName = L"Testing";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	int totalWidth = GetSystemMetrics(SM_CXSCREEN);
	int totalHeight = GetSystemMetrics(SM_CYSCREEN);

	//FULL SCREEN compatibility
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)m_width;
		dmScreenSettings.dmPelsHeight = (unsigned long)m_height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	}
	else //Windowed mode with no borders.
	{
		posX = (totalWidth - m_width) / 2;
		posY = (totalHeight - m_height) / 2;
	}

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, WS_POPUP | WS_VISIBLE | WS_SYSMENU, posX, posY, m_width, m_height, NULL, NULL, m_instance, NULL);
	if (!m_hWnd)
	{
		
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	ShowCursor(false);

	return true;
}

bool BaseApp::InitD3D()
{
	//-------------------------------------------------------------------
	//Needed Variables
	//-------------------------------------------------------------------
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModelList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	//-------------------------------------------------------------------
	//Get PC Information
	//-------------------------------------------------------------------

	if (MSAA_ENABLED)
	{
		//temporarily create device to get multisampling quality
		result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, 
			D3D11_SDK_VERSION, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);
		if (FAILED(result))
		{
			return false;
		}

		result = (m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));
		if (FAILED(result))
		{
			return false;
		}
		assert(m_4xMsaaQuality > 0);

		ReleaseCOM(m_d3dDeviceContext);
		ReleaseCOM(m_d3dDevice);
	}
	

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	displayModelList = new DXGI_MODE_DESC[numModes];
	if (!displayModelList)
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModelList);
	if (FAILED(result))
	{
		return false;
	}

	//Screen Refresh rate
	for (i = 0; i < numModes; i++)
	{
		if (displayModelList[i].Width == (unsigned int)m_width)
		{
			if (displayModelList[i].Height == (unsigned int)m_height)
			{
				numerator = displayModelList[i].RefreshRate.Numerator;
				denominator = displayModelList[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//convert to megabytes
	m_gpuMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	//get GPU name
	wcstombs(m_gpuDesc, adapterDesc.Description, 128);
	
	
	//Releasing
	//*********

	delete[] displayModelList;
	displayModelList = 0;

	ReleaseCOM(adapterOutput);
	ReleaseCOM(adapter);
	ReleaseCOM(factory);

	//-------------------------------------------------------------------
	//Swap Chain
	//-------------------------------------------------------------------
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_width;
	swapChainDesc.BufferDesc.Height = m_height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (VSYNC_ENABLED) 
	{
		//match screen refresh rate
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_hWnd;

	if (MSAA_ENABLED)
	{
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	if (FULL_SCREEN)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, 
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, 
		&m_swapChain, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);

	if (result == E_INVALIDARG)
	{
		result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			&featureLevels[1], _countof(featureLevels) - 1, D3D11_SDK_VERSION, &swapChainDesc,
			&m_swapChain, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);
	}
	if (FAILED(result))
	{
		return false;
	}

	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}
	//Attach back buffer to the swap chain
	result = m_d3dDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	ReleaseCOM(backBufferPtr);

	//-------------------------------------------------------------------
	//Depth Buffer, Depth Stencil
	//-------------------------------------------------------------------

	//---------Depth Buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = m_width;
	depthBufferDesc.Height = m_height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_d3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//------Depth Stencil State
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	m_d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	//---------Depth Stencil View

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	m_d3dDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//-------------------------------------------------------------------
	//Rasterizer
	//-------------------------------------------------------------------

	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	result = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_solidRS);
	if (FAILED(result))
	{
		return false;
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	result = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_wireFrameRS);
	if (FAILED(result))
	{
		return false;
	}

	m_d3dDeviceContext->RSSetState(m_solidRS);

	//Viewport
	//**********

	viewport.Width = (float)m_width;
	viewport.Height = (float)m_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_d3dDeviceContext->RSSetViewports(1, &viewport);

	//-------------------------------------------------------------------
	//Matrix
	//-------------------------------------------------------------------

	//---------Projection
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)m_width / (float)m_height;

	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_DEPTH);

	//---------World
	m_worldMatrix = XMMatrixIdentity();

	//---------Ortho
	m_orthoMatrix = XMMatrixOrthographicLH((float)m_width, (float)m_width, SCREEN_NEAR, SCREEN_DEPTH);


	return true;
}

LRESULT BaseApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				m_appPaused = true;
			}
			else
			{
				m_appPaused = false;
			}
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_GETMINMAXINFO:
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 300;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300;
			return 0;
		}

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}