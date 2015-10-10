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
	Model m_shapes;
	ColorShader m_colorShader;

	float m_radius;
	float m_phi;
	float m_theta;
	float m_cameraX;
	float m_cameraY;
	float m_cameraZ;
	float m_cameraXVel;
	float m_cameraZVel;
	float m_radiusVel;
	float m_movementAngle;

	POINT m_lastMousePos;

	//plain data
	DirectX::XMMATRIX m_plainWorld;
	offsetData m_plainOffset;

	//box data
	DirectX::XMMATRIX m_boxWorld;
	offsetData m_boxOffset;

	//Cylinders data
	DirectX::XMMATRIX m_cylinderWorld[10];
	offsetData m_cylinderOffset[10];

	//spheres data
	DirectX::XMMATRIX m_spheresWorld[10];
	offsetData m_spheresOffset[10];

	//center sphere data
	DirectX::XMMATRIX m_sphereWorld;
	offsetData m_sphereOffset;

	//skull data
	DirectX::XMMATRIX m_skullWorld;
	offsetData m_skullOffset;
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
	m_radius(0),
	m_phi(0),
	m_theta(0),
	m_cameraX(0),
	m_cameraY(0),
	m_cameraZ(0),
	m_cameraXVel(0),
	m_cameraZVel(0),
	m_radiusVel(0),
	m_movementAngle(0)
{
}

SimpleApp::~SimpleApp()
{
	m_colorShader.Shutdown();
	m_shapes.Shutdown();
}

bool SimpleApp::Init()
{
	if (!BaseApp::Init())
	{
		return false;
	}
	//additional init

	m_cameraX = 0.0f;
	m_cameraY = 7.0f;
	m_cameraZ = -15.0f;
	m_radius = 25.0f;
	m_theta = -14.0f;
	m_cameraXVel = 5.0f;
	m_cameraZVel = 5.0f;
	m_radiusVel = 28.8f;
	m_movementAngle = 5.0f;
	
	m_plainOffset = m_shapes.AddGeometry(MODEL_TYPE_PLAIN);
	m_plainWorld = XMMatrixIdentity();

	m_boxOffset = m_shapes.AddGeometry(MODEL_TYPE_CUBE);
	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	m_boxWorld = XMMatrixMultiply(boxScale, boxOffset);

	m_sphereOffset = m_shapes.AddGeometry(MODEL_TYPE_SPHERE);
	XMMATRIX sphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX sphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_sphereWorld = XMMatrixMultiply(sphereScale, sphereOffset);

	m_skullOffset = m_shapes.AddCustomGeometry(L"Resources/skull.txt");
	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.8f, 0.0f);
	m_skullWorld = XMMatrixMultiply(skullScale, skullOffset);

	for (int i = 0; i < 10; ++i)
	{
		m_spheresOffset[i] = m_shapes.AddGeometry(MODEL_TYPE_SPHERE);
		m_cylinderOffset[i] = m_shapes.AddGeometry(MODEL_TYPE_CYLINDER);
	}

	for (int i = 0; i < 5; ++i)
	{
		m_cylinderWorld[i * 2 + 0] = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
		m_cylinderWorld[i * 2 + 1] = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

		m_spheresWorld[i * 2 + 0] = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
		m_spheresWorld[i * 2 + 1] = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);
	}

	if (!m_shapes.Initialize(m_d3dDevice))
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
	m_theta -= m_radiusVel*dt;
	float rx = m_radius;
	float rz = m_phi;
	float ry = m_theta;

	m_camera.SetRotation(rx, ry, rz);


	float r = 15.0f;
	float angleVel = 0.5f;

	m_movementAngle += angleVel * dt;

	m_cameraX = cosf(m_movementAngle) * r;
	m_cameraZ = sinf(m_movementAngle) * r;

	m_camera.SetPosition(m_cameraX, m_cameraY, m_cameraZ);
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

	m_shapes.Render(m_d3dDeviceContext);

	//Render in wireFrame
	m_d3dDeviceContext->RSSetState(m_wireFrameRS);
	
	//render plain
	m_colorShader.Render(m_d3dDeviceContext, m_plainWorld, m_view, m_projectionMatrix, m_plainOffset.indexCount, m_plainOffset.indexOffset, m_plainOffset.vertexOffset);
	//render box
	m_colorShader.Render(m_d3dDeviceContext, m_boxWorld, m_view, m_projectionMatrix, m_boxOffset.indexCount, m_boxOffset.indexOffset, m_boxOffset.vertexOffset);
	//render Sphere
	//m_colorShader.Render(m_d3dDeviceContext, m_sphereWorld, m_view, m_projectionMatrix, m_sphereOffset.indexCount, m_sphereOffset.indexOffset, m_sphereOffset.vertexOffset);
	//render skull
	m_colorShader.Render(m_d3dDeviceContext, m_skullWorld, m_view, m_projectionMatrix, m_skullOffset.indexCount, m_skullOffset.indexOffset, m_skullOffset.vertexOffset);

	for (int i = 0; i < 10; ++i)
	{
		//render 10 of each: Spheres and Cylinders
		m_colorShader.Render(m_d3dDeviceContext, m_spheresWorld[i], m_view, m_projectionMatrix, m_spheresOffset[i].indexCount, m_spheresOffset[i].indexOffset, m_spheresOffset[i].vertexOffset);
		m_colorShader.Render(m_d3dDeviceContext, m_cylinderWorld[i], m_view, m_projectionMatrix, m_cylinderOffset[i].indexCount, m_cylinderOffset[i].indexOffset, m_cylinderOffset[i].vertexOffset);
	}

	m_d3dDeviceContext->RSSetState(m_solidRS);

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