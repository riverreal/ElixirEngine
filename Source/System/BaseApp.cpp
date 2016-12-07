#include "BaseApp.h"
#include "../Helper/GeneralHelper.h"

namespace radix
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return g_baseApp->MsgProc(hwnd, msg, wParam, lParam);
	}

	int BaseApp::m_frameCnt = 0;
	float BaseApp::m_timeElapsed = 0.0f;

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
		//m_depthSRV(0),
		m_solidRS(0),
		m_wireFrameRS(0),
		m_currentScene(0)
	{
		AllocConsole();
		freopen("conin$", "r", stdin);
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
	}

	BaseApp::~BaseApp()
	{
		//Draw related shutdown
		m_shadowMapShader->Shutdown();
		m_shadowMap->Shutdown();

		m_postProcessingShader->Shutdown();

		m_skyShader->Shutdown();

		m_deferredBuffers->Shutdown();
		m_deferredLightShader->Shutdown();
		m_deferredShader->Shutdown();

		SafeRelease(m_shadowMapShader);
		SafeRelease(m_shadowMap);

		SafeRelease(m_postProcessingShader);

		SafeRelease(m_skyShader);
		SafeRelease(m_deferredBuffers);
		SafeRelease(m_deferredLightShader);
		SafeRelease(m_deferredShader);

		m_ortho.Shutdown();

		//D3D Shutdown
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(false, NULL);
		}

		ReleaseCOM(m_solidRS);
		ReleaseCOM(m_cullClockWiseRS);
		ReleaseCOM(m_wireFrameRS);
		ReleaseCOM(m_noDoubleBlendDSS);
		ReleaseCOM(m_solidNoCullRS);
		ReleaseCOM(m_depthStencilView);
		//ReleaseCOM(m_depthSRV);
		ReleaseCOM(m_drawReflecDSS);
		ReleaseCOM(m_markMirrorDSS);
		ReleaseCOM(m_disabledDepthDDS);
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

		if (!InitDraw())
		{
			MessageBox(0, L"Failed Preparing Draw", L"ERROR!", MB_OK);
			return false;
		}

		return true;
	}

	int BaseApp::Run()
	{
		MSG msg;

		ZeroMemory(&msg, sizeof(MSG));

		m_gameTimer.Reset();

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_gameTimer.Tick();

				if (!m_appPaused)
				{
					//do stuff
					displayFPS();

					//fps capping
					if (ENABLE_FPS_CAP)
					{
						if (m_frameCnt < FPS_CAP)
						{
							Frame();
						}
					}
					else
					{
						Frame();
					}
				}
				else
				{
					Sleep(100);
				}
			}
		}

		return (int)msg.wParam;
	}

	void BaseApp::Draw(float dt)
	{
		assert(m_d3dDeviceContext);
		assert(m_swapChain);

		if (m_currentScene != 0 && m_currentScene->IsSceneReady())
		{
			m_currentScene->GetCamera()->Update();
			if (m_currentScene->GetRenderMode() == RENDER_MODE::DEFERRED_RENDERING)
			{
				m_currentScene->GetModel()->Render(m_d3dDeviceContext);
				m_shadowMap->Render(m_d3dDeviceContext);

				for (auto &object : m_currentScene->GetChildren())
				{
					m_d3dDeviceContext->RSSetState(m_solidRS);
					//render if not disabled
					if (object->GetDisabled() == false)
					{
						if (object->GetCastShadow())
						{
							m_shadowMapShader->Render(m_d3dDeviceContext, object, m_currentScene->GetLight());
						}
					}
				}

				m_deferredBuffers->SetRenderTargets(m_d3dDeviceContext);
				m_deferredBuffers->ClearRenderTargets(m_d3dDeviceContext);

				m_d3dDeviceContext->RSSetViewports(1, &m_deferredViewport);

				bool backCulling = true;
				m_d3dDeviceContext->RSSetState(m_solidRS);
				//iterate through every object to render it
				for (auto &object : m_currentScene->GetChildren())
				{
					//render if not disabled
					if (object->GetDisabled() == false)
					{
						if (object->GetBackFaceCulling() == false)
						{
							//if last setting was the same skip raaster state setting
							if (backCulling)
							{
								m_d3dDeviceContext->RSSetState(m_solidNoCullRS);
								backCulling = false;
							}
							
						}
						else
						{
							if (!backCulling)
							{
								m_d3dDeviceContext->RSSetState(m_solidRS);
								backCulling = true;
							}
						}

						m_deferredShader->Render(m_d3dDeviceContext, object, m_currentScene->GetCamera());
					}
				}

				m_d3dDeviceContext->RSSetViewports(1, &m_deferredSkyViewport);

				//To render inner sphere
				m_d3dDeviceContext->RSSetState(m_solidNoCullRS);
				m_skyShader->Render(m_d3dDeviceContext, m_currentScene->GetSky(), m_currentScene->GetCamera());
				m_d3dDeviceContext->RSSetState(m_solidRS);
				m_d3dDeviceContext->RSSetViewports(1, &m_deferredViewport);

				//m_deferredBuffers->SetPostpRenderTarget(m_d3dDeviceContext);
				//Render to aux 0
				m_postProcessingShader->SetPostPRenderTarget(m_d3dDeviceContext);
				//m_deferredBuffers->ClearPostpRenderTarget(m_d3dDeviceContext);
				m_postProcessingShader->ClearPostPRenderTarget(m_d3dDeviceContext);
				m_ortho.Render(m_d3dDeviceContext);
				m_deferredLightShader->Render(m_d3dDeviceContext, offsetData(m_ortho.GetIndexCount(), 0, 0), m_currentScene->GetEnvMap(), m_currentScene->GetIrradiance(), m_deferredBuffers->GetShaderResourceView(0), 
					m_deferredBuffers->GetShaderResourceView(1), m_deferredBuffers->GetShaderResourceView(2), m_deferredBuffers->GetShaderResourceView(3), m_shadowMap->GetShadowMap(),
					m_currentScene->GetLight(), m_currentScene->GetCamera(), m_currentScene->GetFog());

				//default render target
				SetDefaultRenderTargetOn();
				float color[4] = { 255, 255, 255, 255 };
				m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, color);
				m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				//Faster post processing fullscreen triangle
				m_d3dDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
				m_d3dDeviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
				m_d3dDeviceContext->IASetInputLayout(NULL);
				m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//pass dt for some time-based transition effects
				m_postProcessingShader->Render(m_d3dDeviceContext, offsetData(m_ortho.GetIndexCount(), 0, 0), dt);
			}
		}

		//-------render end
		if (VSYNC_ENABLED)
		{
			m_swapChain->Present(1, 0);
		}
		else
		{
			m_swapChain->Present(0, 0);
		}
	}

	void BaseApp::Frame()
	{
		m_frameCnt++;
		Update(m_gameTimer.DeltaTime());
		Draw(m_gameTimer.DeltaTime());
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

		
		

		//default window style (windowed mode)
		DWORD windowStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_CAPTION;

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
			windowStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
		}
		else //Windowed mode with no borders.
		{
			posX = (totalWidth - m_width) / 2;
			posY = (totalHeight - m_height) / 2;
		}

		RECT clientSize = {0, 0, m_width, m_height};
		AdjustWindowRect(&clientSize, windowStyle, false);

		m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, windowStyle, posX, posY, clientSize.right- clientSize.left, clientSize.bottom- clientSize.top, NULL, NULL, m_instance, NULL);
		if (!m_hWnd)
		{
		
			return false;
		}

		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
		if(HIDE_CURSOR)
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
		std::vector <IDXGIAdapter*> vAdapters;
		IDXGIOutput* adapterOutput;
		unsigned int numModes, i, numerator, denominator;
		DXGI_MODE_DESC* displayModelList;
		DXGI_ADAPTER_DESC adapterDesc;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;
		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc;
		D3D11_RASTERIZER_DESC rasterizerDesc;
		D3D11_VIEWPORT viewport;
		float fieldOfView, screenAspect;

		unsigned int adapterNum;

		//Get the real client rect to avoid dedicated graphics client size errors when initializing dx11
		//Source: Gamedev.net (User: NightCreature83)
		//"If you dont pass it the actual GetClientRect window size the problem I was seeing with Dedicated GPUs 
		//was that the compositing and final output to the window was actually still happening on the iGPU instead of on the dGPU."

		RECT realClientSize;
		GetClientRect(m_hWnd, &realClientSize);

		//-------------------------------------------------------------------
		//Get PC Information
		//-------------------------------------------------------------------

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}

		for (UINT j = 0; factory->EnumAdapters(j, &adapter) != DXGI_ERROR_NOT_FOUND; ++j)
		{
			vAdapters.push_back(adapter);
			std::wostringstream outs;
			outs.precision(0);
			outs << j;
			//MessageBox(0, outs.str().c_str(), L"Iteration", MB_OK);
		}

		//vAdapters.pop_back();

		result = vAdapters[0]->EnumOutputs(0, &adapterOutput);
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
			if (displayModelList[i].Width == (unsigned int) m_width)
			{
				if (displayModelList[i].Height == (unsigned int) m_height)
				{
					numerator = displayModelList[i].RefreshRate.Numerator;
					denominator = displayModelList[i].RefreshRate.Denominator;
				}
			}
		}
	
		//Get the video ram from every adapter
		std::vector<int> vRam;
		for (auto &adapter : vAdapters)
		{
			result = adapter->GetDesc(&adapterDesc);
			if (FAILED(result))
			{
			}
			else
			{
				vRam.push_back(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
			}
		}

		//Simple loop to get greatest video ram
		int greatest = 0;
		int iteration = 0;
		for (auto &memory : vRam)
		{
			if (memory > greatest)
			{
				greatest = memory;
				//Select the best video card by choosing the one with largest vram
				adapterNum = iteration;
			}

			iteration++;
		}

		result = vAdapters[adapterNum]->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			return false;
		}

		//MSAA support quality
		if (MSAA_ENABLED)
		{
			//temporarily create device to get multisampling quality
			result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0,
				D3D11_SDK_VERSION, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);
			if (FAILED(result))
			{
				return false;
			}

			if (featureLevel != D3D_FEATURE_LEVEL_11_0)
			{
				MessageBox(0, L"D3D Feature Level 11 not supported", L"Error", MB_OK);
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

		std::wostringstream outs;
		outs.precision(0);
		outs << L"GPU: " << m_gpuDesc << L"   " << m_gpuMemory << L"MB";
		//MessageBox(0, outs.str().c_str(), L"Info", MB_OK);

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

		result = D3D11CreateDeviceAndSwapChain(vAdapters[adapterNum], D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
			featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, 
			&m_swapChain, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);

		if (result == E_INVALIDARG)
		{
			result = D3D11CreateDeviceAndSwapChain(vAdapters[adapterNum], D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
				&featureLevels[1], _countof(featureLevels) - 1, D3D11_SDK_VERSION, &swapChainDesc,
				&m_swapChain, &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);
			MessageBox(0, L"Changing Feature Level to 11_0", L"Feature Level 11_1 not Supported", MB_OK);
		}
		if (FAILED(result))
		{
			MessageBox(0, L"Unable to create Device and SwapChain", L"Error", MB_OK);
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
		depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	
		if (MSAA_ENABLED)
		{
			depthBufferDesc.SampleDesc.Count = 4;
			depthBufferDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{

			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
		}
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		result = m_d3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth buffer", 0, 0);
			return false;
		}

		//------Depth Stencil State
		//Default
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
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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

		result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_skyDSS);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = false;
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

		result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_disabledDepthDDS);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}

		//No Double Blending (shadows)
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_noDoubleBlendDSS);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}


		//Mark mirror Depth Stencil State
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_markMirrorDSS);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}

		//Draw Reflection Depth Stencil State
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		result = m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_drawReflecDSS);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil", 0, 0);
			return false;
		}

		//Set Default Depth Stencil State
		m_d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


		//---------Depth Stencil View
		ZeroMemory(&depthSRVDesc, sizeof(depthSRVDesc));
		depthSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthSRVDesc.Texture2D.MostDetailedMip = 0;
		depthSRVDesc.Texture2D.MipLevels = -1;

		//result = m_d3dDevice->CreateShaderResourceView(m_depthStencilBuffer, &depthSRVDesc, &m_depthSRV);
		//if (FAILED(result))
		//{
		//	MessageBox(0, L"Failed to create depth SRV", 0, 0);
		//	return false;
		//}

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
	

		result = m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result))
		{
			MessageBox(0, L"Failed to create depth stencil view", 0, 0);
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
		if (MSAA_ENABLED)
		{
			rasterizerDesc.MultisampleEnable = true;
		}
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_solidRS);
		if (FAILED(result))
		{
			return false;
		}

		rasterizerDesc.FrontCounterClockwise = true;
		result = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_cullClockWiseRS);
		if (FAILED(result))
		{
			return false;
		}
		rasterizerDesc.FrontCounterClockwise = false;

		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		result = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_solidNoCullRS);
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

		//default Rasterizer mode
		m_d3dDeviceContext->RSSetState(m_solidRS);

		//Viewport
		//**********

		m_defaultViewport.Width = (float)m_width;
		m_defaultViewport.Height = (float)m_height;
		m_defaultViewport.MinDepth = 0.0f;
		m_defaultViewport.MaxDepth = 1.0f;
		m_defaultViewport.TopLeftX = 0.0f;
		m_defaultViewport.TopLeftY = 0.0f;

		m_d3dDeviceContext->RSSetViewports(1, &m_defaultViewport);

		XMFLOAT2 specResolution = GetSpecResolution(m_width, m_height);
		m_deferredViewport.Width = (float)specResolution.x;
		m_deferredViewport.Height = (float)specResolution.y;
		m_deferredViewport.MinDepth = 0.0f;
		m_deferredViewport.MaxDepth = 0.9f;
		m_deferredViewport.TopLeftX = 0.0f;
		m_deferredViewport.TopLeftY = 0.0f;

		m_deferredSkyViewport.Width = (float)specResolution.x;
		m_deferredSkyViewport.Height = (float)specResolution.y;
		m_deferredSkyViewport.MinDepth = 0.9f;
		m_deferredSkyViewport.MaxDepth = 1.0f;
		m_deferredSkyViewport.TopLeftX = 0.0f;
		m_deferredSkyViewport.TopLeftY = 0.0f;

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
		m_orthoMatrix = XMMatrixOrthographicLH((float)m_width, (float)m_height, SCREEN_NEAR, SCREEN_DEPTH);

		return true;
	}

	bool BaseApp::InitDraw()
	{
		m_deferredBuffers = new DeferredRendering();
		XMFLOAT2 specResolution = GetSpecResolution(m_width, m_height);
		if (!m_deferredBuffers->Initialize(m_d3dDevice, m_width, m_height, SCREEN_DEPTH, SCREEN_NEAR, SPEC_INDEX))
		{
			MessageBox(0, L"Cant initalize deferred buffers", L"Error", MB_OK);
			return false;
		}

		m_deferredLightShader = new DeferredLightShader();
		if (!m_deferredLightShader->Initialize(m_d3dDevice, m_hWnd))
		{
			MessageBox(0, L"Cant initalize deferred light shader", L"Error", MB_OK);
			return false;
		}

		m_deferredShader = new DeferredShader();
		if (!m_deferredShader->Initialize(m_d3dDevice, m_hWnd))
		{
			MessageBox(0, L"Cant initalize deferred shader", L"Error", MB_OK);
			return false;
		}

		m_skyShader = new SkyDome();
		if (!m_skyShader->Initialize(m_d3dDevice, m_hWnd))
		{
			MessageBox(0, L"Can't initialize sky shader", L"Error", MB_OK);
			return false;
		}

		m_postProcessingShader = new PostProcessShader();
		if (!m_postProcessingShader->Initialize(m_d3dDevice, m_hWnd, m_width, m_height, SCREEN_DEPTH, SCREEN_NEAR, m_defaultViewport))
		{
			MessageBox(0, L"Can't initialize Post Process Shader", L"Error", MB_OK);
			return false;
		}

		if(!m_ortho.Initialize(m_d3dDevice, m_width, m_height))
		{
			MessageBox(0, L"Cant initalize ortho manager", L"Error", MB_OK);
			return false;
		}

		m_shadowMap = new ShadowMap();
		if (!m_shadowMap->Initialize(m_d3dDevice, 2048, 2048))
		{
			MessageBox(0, L"Can't initialize ShadowMap", L"Error", MB_OK);
			return false;
		}

		m_shadowMapShader = new ShadowMapShader();
		if (!m_shadowMapShader->Initialize(m_d3dDevice))
		{
			MessageBox(0, L"Can't initialize ShadowMapShader", L"Error", MB_OK);
			return false;
		}

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
					m_gameTimer.Stop();
				}
				else
				{
					m_appPaused = false;
					m_gameTimer.Start();
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
			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE)
				{
					PostQuitMessage(0);
					return 0;
				}
				return 0;
			}
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			case WM_MOUSEMOVE:
				OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void BaseApp::displayFPS()
	{
		//Refresh FPS each second
		if (m_gameTimer.TotalTime() - m_timeElapsed >= 1.0f)
		{
			float fps = (float)m_frameCnt;
			float mspf = 1000.0f / fps;

			std::wostringstream outs;
			outs.precision(0);
			outs << m_appName << L"    "
				<< L"FPS: " << fps << L"     "
				<< L"GPU: " << m_gpuDesc;
			SetWindowText(m_hWnd, outs.str().c_str());

			m_frameCnt = 0;
			m_timeElapsed += 1.0f;
		}
	}

	void BaseApp::SetZBufferOn()
	{
		m_d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	}

	void BaseApp::SetZBufferOff()
	{
		m_d3dDeviceContext->OMSetDepthStencilState(m_disabledDepthDDS, 1);
	}

	void BaseApp::SetDefaultRenderTargetOn()
	{
		m_d3dDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	}

	XMFLOAT2 BaseApp::GetSpecResolution(int screenWidth, int screenHeight)
	{
		int width, height;
		if (SPEC_INDEX == 0) //Ultra
		{
			//No resolution down grade
			width = screenWidth;
			height = screenHeight;
		}
		else if (SPEC_INDEX == 1) //High
		{
			if ((screenWidth / screenHeight) == (16 / 9))
			{
				//One resolution down grade.
				if (screenWidth > 1920)
				{
					width = 1920;
					height = 1080;
				}
				else if (screenWidth == 1920)
				{
					width = 1280;
					height = 720;
				}
				else
				{
					width = 720;
					height = 480;
				}
			}
			else
			{
				width = screenWidth;
				height = screenHeight;
			}
		}
		else //low
		{
			if ((screenWidth / screenHeight) == (16 / 9))
			{
				//two resolution down grade.
				if (screenWidth > 1920)
				{
					width = 1280;
					height = 720;
				}
				else if (screenWidth == 1920)
				{
					width = 1280;
					height = 720;
				}
				else
				{
					width = 720;
					height = 480;
				}
			}
			else
			{
				width = screenWidth;
				height = screenHeight;
			}
		}
		return DirectX::XMFLOAT2(width, height);
	}

	void BaseApp::SetScene(Scene * scene)
	{
		m_currentScene = scene;
	}

}