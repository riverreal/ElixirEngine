#include "System\BaseApp.h"
#include "Graphics\ModelManager.h"
#include "Graphics\ColorShader.h"
#include "Graphics\LightShader.h"
#include "Graphics\CameraManager.h"
#include "Graphics\TextureLoader.h"

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
	LightShader m_colorShader;
	LightShader m_lightShader;
	LightShader m_boxShader;
	LightShader m_cylinderShader;
	LightShader m_sphereShader;
	
	ID3D11ShaderResourceView* m_crateTexture;
	ID3D11ShaderResourceView* m_grassTexture;
	DirectX::XMMATRIX m_defTexTranf;
	DirectX::XMMATRIX m_grassTexTransf;
	float m_grassX;
	float m_grassY;

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
	float m_skullAngle;
	XMFLOAT3 m_pointBulbPos;

	POINT m_lastMousePos;

	//Light
	BasicLight m_basicLight;

	//Materials
	Material m_simpleMaterial;

	//plain data
	DirectX::XMMATRIX m_plainWorld;
	offsetData m_plainOffset;
	Material m_plainMaterial;

	//box data
	DirectX::XMMATRIX m_boxWorld;
	offsetData m_boxOffset;
	Material m_boxMaterial;

	//Cylinders data
	DirectX::XMMATRIX m_cylinderWorld[10];
	offsetData m_cylinderOffset[10];
	Material m_cylinderMaterial;

	//spheres data
	DirectX::XMMATRIX m_spheresWorld[11];
	offsetData m_spheresOffset[11];
	Material m_sphereMaterial;

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
	m_movementAngle(0),
	m_skullAngle(0)
{
}

SimpleApp::~SimpleApp()
{
	m_colorShader.Shutdown();
	m_lightShader.Shutdown();
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
	m_cameraZ = -13.0f;
	m_radius = 25.0f;
	m_theta = -14.0f;
	m_cameraXVel = 5.0f;
	m_cameraZVel = 5.0f;
	m_radiusVel = 28.8f;
	m_movementAngle = 5.0f;
	m_skullAngle = 5.0f;
	m_pointBulbPos.x = 0.0f;
	m_pointBulbPos.y = 5.0f;
	m_pointBulbPos.z = 5.0f;

	m_defTexTranf = XMMatrixIdentity();
	m_grassX = 1.0f;
	m_grassY = 1.0f;
	m_grassTexTransf = XMMatrixMultiply(m_defTexTranf, XMMatrixScaling(5.0f, 5.0f, 5.0f));

	m_crateTexture = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/WoodCrate01.dds");
	m_grassTexture = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/water1.dds");

	m_plainOffset = m_shapes.AddGeometry(MODEL_TYPE_PLAIN);
	m_plainWorld = XMMatrixIdentity();

	m_spheresOffset[10] = m_shapes.AddGeometry(MODEL_TYPE_GEOSPHERE);
	XMMATRIX plbScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX plbPos = XMMatrixTranslation(m_pointBulbPos.x, m_pointBulbPos.y, m_pointBulbPos.z);
	m_spheresWorld[10] = XMMatrixMultiply(plbScale, plbPos);

	m_boxOffset = m_shapes.AddGeometry(MODEL_TYPE_CUBE);
	XMMATRIX boxScale = XMMatrixScaling(2.5f, 2.5f, 2.5f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 2.5f, 0.0f);
	m_boxWorld = XMMatrixMultiply(boxScale, boxOffset);

	m_sphereOffset = m_shapes.AddGeometry(MODEL_TYPE_SPHERE);
	XMMATRIX sphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX sphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_sphereWorld = XMMatrixMultiply(sphereScale, sphereOffset);

	//m_skullOffset = m_shapes.AddCustomGeometry(L"Resources/skull.txt");
	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.8f, 0.0f);
	m_skullWorld = XMMatrixMultiply(skullScale, skullOffset);

	//Material data
	m_boxMaterial.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_boxMaterial.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_boxMaterial.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

	m_sphereMaterial.Ambient = XMFLOAT4(0.5f, 0.5f, 0.1f, 1.0f);
	m_sphereMaterial.Diffuse = XMFLOAT4(0.8f, 0.7f, 0.3f, 1.0f);
	m_sphereMaterial.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 2.0f);

	m_cylinderMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_cylinderMaterial.Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_cylinderMaterial.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	m_plainMaterial.Ambient = XMFLOAT4(0.48, 0.77f, 0.46f, 1.0f);
	m_plainMaterial.Diffuse = XMFLOAT4(0.48, 0.77f, 0.46f, 1.0f);
	m_plainMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_simpleMaterial.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_simpleMaterial.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_simpleMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f);

	//light data
	m_basicLight.Directional.Ambient = XMFLOAT4(0.5f, 0.45f, 0.4f, 1.0f);
	m_basicLight.Directional.Diffuse = XMFLOAT4(0.95f, 0.90f, 0.85f, 1.0f);
	m_basicLight.Directional.Specular = XMFLOAT4(0.5f, 0.5f, 0.35f, 1.0f);
	m_basicLight.Directional.Direction = XMFLOAT3(-0.707f, -1.0f, 0.7f);
	/*
	m_basicLight.Spot.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_basicLight.Spot.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_basicLight.Spot.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_basicLight.Spot.Att = XMFLOAT3(0.3f, 0.0f, 0.0f);
	m_basicLight.Spot.Spot = 400.0f;
	m_basicLight.Spot.Range = 1000.0f;
	m_basicLight.Spot.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	//m_basicLight.Spot.Position = XMFLOAT3(0.0f, 10.0f, 10.0f);
	*/
	/*
	m_basicLight.Point.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_basicLight.Point.Diffuse = XMFLOAT4(0.0f, 0.1f, 1.0f, 1.0f);
	m_basicLight.Point.Specular =XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_basicLight.Point.Att = XMFLOAT3(1.0f, 0.1f, 0.01f);
	m_basicLight.Point.Range = 500.0f;
	m_basicLight.Point.Position = XMFLOAT3(m_pointBulbPos);
	*/
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

	if (!m_colorShader.Initialize(m_d3dDevice, m_hWnd, m_plainMaterial))
	{
		return false;
	}

	if (!m_lightShader.Initialize(m_d3dDevice, m_hWnd, m_simpleMaterial))
	{
		return false;
	}

	if (!m_cylinderShader.Initialize(m_d3dDevice, m_hWnd, m_cylinderMaterial))
	{
		return false;
	}

	if (!m_sphereShader.Initialize(m_d3dDevice, m_hWnd, m_sphereMaterial))
	{
		return false;
	}
	
	if (!m_boxShader.Initialize(m_d3dDevice, m_hWnd, m_boxMaterial))
	{
		return false;
	}

	return true;
}

void SimpleApp::Update(float dt)
{
	//Camera rotation
	//m_theta -= m_radiusVel*dt;
	float rx = m_radius;
	float rz = m_phi;
	float ry = m_theta;
	m_camera.SetRotation(rx, ry, rz);
	//Camera movement
	float r = 15.0f;
	float angleVel = 0.5f;
	m_movementAngle += angleVel * dt;
	//m_cameraX = cosf(m_movementAngle) * r;
	//m_cameraZ = sinf(m_movementAngle) * r;
	m_camera.SetPosition(m_cameraX, m_cameraY, m_cameraZ);

	//light rotation
	XMVECTOR pos = XMVectorSet(m_cameraX, m_cameraY, m_cameraZ, 1.0f);
	XMVECTOR target = XMVectorZero();
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;
	XMFLOAT3 lookAt;
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	target = XMLoadFloat3(&lookAt);
	pitch = rx * 0.0174532925f;
	yaw = ry * 0.0174532925f;
	roll = rz * 0.0174532925f;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	target = XMVector3TransformCoord(target, rotationMatrix);
	m_basicLight.Spot.Position = XMFLOAT3(m_cameraX, m_cameraY, m_cameraZ);
	XMStoreFloat3(&m_basicLight.Spot.Direction, XMVector3Normalize(target - pos));
	//m_basicLight.Spot.Direction = m_camera.GetRotation();
	
	m_grassX += 0.05f * dt;
	m_grassY += 0.01f * dt;
	XMMATRIX grassOffset = XMMatrixTranslation(m_grassX, m_grassY, 0.0f);
	XMMATRIX grassScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	m_grassTexTransf = XMMatrixMultiply(grassScale, grassOffset);
}

void SimpleApp::Draw()
{
	assert(m_d3dDeviceContext);
	assert(m_swapChain);

	float color[4] = {0, 0, 0, 255};

	m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, color);

	m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	XMMATRIX m_view;

	m_camera.Render();
	m_camera.GetViewMatrix(m_view);

	m_shapes.Render(m_d3dDeviceContext);

	//Render in wireFrame
	//m_d3dDeviceContext->RSSetState(m_wireFrameRS);
	
	
	//render plain
	m_colorShader.Render(m_d3dDeviceContext, m_plainWorld, m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_grassTexture, m_grassTexTransf, m_plainOffset.indexCount, m_plainOffset.indexOffset, m_plainOffset.vertexOffset);
	//render box
	m_boxShader.Render(m_d3dDeviceContext, m_boxWorld, m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_crateTexture, m_defTexTranf, m_boxOffset.indexCount, m_boxOffset.indexOffset, m_boxOffset.vertexOffset);
	//render Sphere
	//m_colorShader.Render(m_d3dDeviceContext, m_sphereWorld, m_view, m_projectionMatrix, m_crateTexture, m_sphereOffset.indexCount, m_sphereOffset.indexOffset, m_sphereOffset.vertexOffset);

	for (int i = 0; i < 10; ++i)
	{
		//render 10 of each: Spheres and Cylinders
		m_sphereShader.Render(m_d3dDeviceContext, m_spheresWorld[i], m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_crateTexture, m_defTexTranf, m_spheresOffset[i].indexCount, m_spheresOffset[i].indexOffset, m_spheresOffset[i].vertexOffset);
		m_cylinderShader.Render(m_d3dDeviceContext, m_cylinderWorld[i], m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_crateTexture, m_defTexTranf, m_cylinderOffset[i].indexCount, m_cylinderOffset[i].indexOffset, m_cylinderOffset[i].vertexOffset);
	}
	
	m_boxShader.Render(m_d3dDeviceContext, m_spheresWorld[10], m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_crateTexture, m_defTexTranf, m_spheresOffset[10].indexCount, m_spheresOffset[10].indexOffset, m_spheresOffset[10].vertexOffset);

	//m_d3dDeviceContext->RSSetState(m_solidRS);
	//m_lightShader.Render(m_d3dDeviceContext, m_skullWorld, m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_crateTexture, m_skullOffset.indexCount, m_skullOffset.indexOffset, m_skullOffset.vertexOffset);
	

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