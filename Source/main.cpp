#include "System\BaseApp.h"
#include "Graphics\ModelManager.h"
#include "Graphics\ColorShader.h"
#include "Graphics\CameraManager.h"

#define _USE_MATH_DEFINES

#include <math.h>

const int SCREEN_WIDTH = 1280;
const int SCRENN_HEIGHT = 720;

class SimpleApp : public BaseApp
{
public:
	SimpleApp(HINSTANCE instance, int width, int height);
	~SimpleApp();

	bool Init() override;
	void Update(float dt) override;
	void Draw() override;
	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
private:
	Camera m_camera;
	Model m_box;
	ColorShader m_colorShader;

	float m_radius;
	float m_phi;
	float m_theta;

	POINT m_lastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SimpleApp myGame(hInstance, SCREEN_WIDTH, SCRENN_HEIGHT);

	if (!myGame.Init())
	{
		return 0;
	}

	myGame.Run();

	return 0;
}


SimpleApp::SimpleApp(HINSTANCE instance, int width, int height)
	:BaseApp(instance, width, height),
	m_box(BasicModel::MODEL_TYPE_PLAIN),
	m_radius(0),
	m_phi(0),
	m_theta(0)
{
}

SimpleApp::~SimpleApp()
{
	m_colorShader.Shutdown();
	m_box.Shutdown();
}

bool SimpleApp::Init()
{
	if (!BaseApp::Init())
	{
		return false;
	}
	//additional init

	m_camera.SetPosition(0.0f, 130.0f, -130.0f);

	if (!m_box.Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_colorShader.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	return true;
}

void SimpleApp::Update(float dt)
{
	float x = m_radius;
	float z = m_phi;
	float y = m_theta;

	m_camera.SetRotation(x, y, z);
}

void SimpleApp::Draw()
{
	assert(m_d3dDeviceContext);
	assert(m_swapChain);

	float color[4] = {255, 255, 255, 255};

	m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, color);

	m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	XMMATRIX m_view;

	m_camera.Render();
	m_camera.GetViewMatrix(m_view);

	m_box.Render(m_d3dDeviceContext);
	m_colorShader.Render(m_d3dDeviceContext, m_box.GetIndexCount(), m_worldMatrix, m_view, m_projectionMatrix);


	if (VSYNC_ENABLED)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

void SimpleApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;

	SetCapture(m_hWnd);
}

void SimpleApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SimpleApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(static_cast<float>(y - m_lastMousePos.y));

		m_theta -= dx;
		m_radius += dy;
	}
	else if( (btnState & MK_RBUTTON) != 0)
	{
		//float dx = 0.005f*static_cast<float>(x - m_lastMousePos.x);
		//float dy = 0.005f*static_cast<float>(y - m_lastMousePos.y);

		//m_radius += dx - dy;
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}