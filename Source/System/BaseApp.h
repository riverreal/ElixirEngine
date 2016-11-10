#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <sstream>
#include <vector>
#include "Timer.h"
#include "RadixScene.h"
#include "../Includes/RDeferred.h"
#include "../Graphics/OrthoManager.h"
#include "../Graphics/Post-processingShader.h"
#include "../Graphics/SkyDome.h"

using namespace DirectX;

namespace radix
{
	const bool FULL_SCREEN = false;
	//const bool RESIZEABLE = false; ------------- Feature Not Added
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;
	const bool VSYNC_ENABLED = true;
	const bool MSAA_ENABLED = false; //4X MSAA --- for forward rendering
	const float FPS_CAP = 60.0f; //When Vsync is disabled --- NOT WORKING
	const bool ENABLE_FPS_CAP = false; //NOT WORKING
	const bool HIDE_CURSOR = false;
	const int SPEC_INDEX = 0; //0 ULTRA / 1 HIGH / 2 LOW

	class BaseApp
	{
	public:
		BaseApp(HINSTANCE instance, int width, int height);
		virtual ~BaseApp();

		virtual bool Init();
		virtual void Update(float dt) = 0;
		virtual void Draw(float dt);
		virtual void Frame();
		int Run();

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
		virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
		virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

	protected:
		bool InitWindow();
		bool InitD3D();
		bool InitDraw();
		void displayFPS(); //in Caption
		void SetZBufferOn();
		void SetZBufferOff();
		void SetDefaultRenderTargetOn();
		DirectX::XMFLOAT2 GetSpecResolution(int screenWidth, int screenHeight);
		void SetScene(Scene* scene);

	protected:
		HINSTANCE m_instance;
		HWND m_hWnd;
		//window size
		int m_width;
		int m_height;

		//in megabytes
		int m_gpuMemory;
		//stores gpu name
		char m_gpuDesc[128];
		//stores 4xMSAA quality
		UINT m_4xMsaaQuality;

		LPCWSTR m_appName;
		LPCWSTR m_caption;

		bool m_resizing; //not used
		bool m_appPaused;

		Timer m_gameTimer;
		static int m_frameCnt;
		static float m_timeElapsed;

		//d3d related
		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_d3dDevice;
		ID3D11DeviceContext* m_d3dDeviceContext;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthStencilBuffer;
		//default
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilState* m_skyDSS;
		ID3D11DepthStencilState* m_disabledDepthDDS;
		//for mirror rendering
		ID3D11DepthStencilState* m_markMirrorDSS;
		//for mirror rendering
		ID3D11DepthStencilState* m_drawReflecDSS;
		//For preventing double blending (shadows)
		//Specify StencilRef to 0 when setting Stencil State
		ID3D11DepthStencilState* m_noDoubleBlendDSS;
		ID3D11DepthStencilView* m_depthStencilView;
		//ID3D11ShaderResourceView* m_depthSRV;
		//default
		ID3D11RasterizerState* m_solidRS;
		ID3D11RasterizerState* m_wireFrameRS;
		//for transparent objects
		ID3D11RasterizerState* m_solidNoCullRS;
		//for reflected objects
		ID3D11RasterizerState* m_cullClockWiseRS;

		//viewports
		D3D11_VIEWPORT m_defaultViewport;
		D3D11_VIEWPORT m_deferredViewport;
		D3D11_VIEWPORT m_deferredSkyViewport;

		DirectX::XMMATRIX m_projectionMatrix;
		DirectX::XMMATRIX m_worldMatrix;
		DirectX::XMMATRIX m_orthoMatrix;

		Scene* m_currentScene;

		//Draw related----

		OrthoManager m_ortho;

		DeferredRendering* m_deferredBuffers;
		DeferredLightShader* m_deferredLightShader;
		DeferredShader* m_deferredShader;
		PostProcessShader* m_postProcessingShader;
		SkyDome* m_skyShader;
	};

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BaseApp* g_baseApp = 0;
}