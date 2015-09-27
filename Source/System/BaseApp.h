#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

const bool FULL_SCREEN = false;
//const bool RESIZEABLE = false; ------------- Feature Not Added
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const bool VSYNC_ENABLED = true;
const bool MSAA_ENABLED = false; //4X MSAA

class BaseApp
{
public:
	BaseApp(HINSTANCE instance, int width, int height);
	virtual ~BaseApp();

	virtual bool Init();
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	void Run();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	bool InitWindow();
	bool InitD3D();

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
	UINT m_4xMsaaQuality;

	LPCWSTR m_appName;

	bool m_resizing;
	bool m_appPaused;

	//d3d related
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dDeviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_solidRS;
	ID3D11RasterizerState* m_wireFrameRS;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BaseApp* g_baseApp = 0;