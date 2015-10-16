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

	bool SceneInit();
private:
	Camera m_camera;
	Model m_shapes;

	ID3D11ShaderResourceView* m_texCrate;
	ID3D11ShaderResourceView* m_texGrass;
	ID3D11ShaderResourceView* m_texWater;
	ID3D11ShaderResourceView* m_texBrick;
	DirectX::XMMATRIX m_texTranfDef;
	DirectX::XMMATRIX m_texTranfCrate;
	DirectX::XMMATRIX m_texTransfGrass;
	DirectX::XMMATRIX m_texTransfWater;
	DirectX::XMMATRIX m_texTransfBrick;

	//Light
	BasicLight m_basicLight;

	//Materials
	Material m_matShiny;
	Material m_matRough;
	Material m_matWater;

	//Renderers
	LightShader m_rendererShiny;
	LightShader m_rendererRough;
	LightShader m_rendererWater;

	//Geometry
	offsetData m_offsetPlain;
	DirectX::XMMATRIX m_worldPlain;

	offsetData m_offsetCont[5];
	DirectX::XMMATRIX m_worldCont[5];

	offsetData m_offsetWater;
	DirectX::XMMATRIX m_worldWater;

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
	XMFLOAT3 m_pointBulbPos;
	float m_waterX;
	float m_waterY;

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
	m_rendererWater.Shutdown();
	m_rendererWater.Shutdown();
	m_rendererShiny.Shutdown();
	m_shapes.Shutdown();
}

bool SimpleApp::SceneInit()
{
	//Vertor based Variables
	m_cameraX = 0.0f;
	m_cameraY = 8.0f;
	m_cameraZ = -15.0f;
	m_radius = 25.0f;
	m_theta = -14.0f;
	m_cameraXVel = 5.0f;
	m_cameraZVel = 5.0f;
	m_radiusVel = 28.8f;
	m_movementAngle = 5.0f;
	m_pointBulbPos.x = 0.0f;
	m_pointBulbPos.y = 5.0f;
	m_pointBulbPos.z = 5.0f;

	m_waterX = 1.0f;
	m_waterY = 1.0f;

	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	m_texTranfDef = XMMatrixIdentity();
	m_texTransfGrass = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(50.0f, 50.0f, 1.0f));
	m_texTranfCrate = m_texTranfDef;
	m_texTransfBrick = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	m_texTransfWater = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(1.0f, 1.0f, 1.0f));

	m_texCrate = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/WoodCrate01.dds");
	m_texWater = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/water2.dds");
	m_texGrass = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/grass.dds");
	m_texBrick = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/darkbrickdxt1.dds");

	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------
	m_basicLight.Directional.Ambient = XMFLOAT4(0.6f, 0.5f, 0.4f, 1.0f);
	m_basicLight.Directional.Diffuse = XMFLOAT4(1.0f, 0.95f, 0.85f, 1.0f);
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
	m_basicLight.Point.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f);
	m_basicLight.Point.Specular =XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_basicLight.Point.Att = XMFLOAT3(1.0f, 0.1f, 0.01f);
	m_basicLight.Point.Range = 200.0f;
	m_basicLight.Point.Position = XMFLOAT3(m_pointBulbPos);
	*/

	//-----------------------------------------------------------------------------------------------------
	//        Material Init
	//-----------------------------------------------------------------------------------------------------
	m_matShiny.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_matShiny.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_matShiny.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 20.0f);

	m_matRough.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_matRough.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_matRough.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_matWater.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_matWater.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_matWater.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 10.0f);

	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------

	m_offsetPlain = m_shapes.AddGeometry(MODEL_TYPE_PLAIN);

	for (int i = 0; i < 5; ++i)
	{
		m_offsetCont[i] = m_shapes.AddGeometry(MODEL_TYPE_CUBE);
	}

	m_offsetWater = m_shapes.AddGeometry(MODEL_TYPE_PLAIN);

	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------

	m_worldPlain = XMMatrixMultiply(XMMatrixScaling(30.0f, 1.0f, 30.0f), XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	m_worldCont[0] = XMMatrixMultiply(XMMatrixScaling(5.0f, 2.0f, 5.0f), XMMatrixTranslation(0.0f, 2.0f, -7.0f));
	m_worldCont[1] = XMMatrixMultiply(XMMatrixScaling(1.0f, 2.0f, 5.0f), XMMatrixTranslation(2.5f, 4.0f, -7.0f));
	m_worldCont[2] = XMMatrixMultiply(XMMatrixScaling(1.0f, 2.0f, 5.0f), XMMatrixTranslation(-2.5f, 4.0f, -7.0f));
	m_worldCont[3] = XMMatrixMultiply(XMMatrixScaling(4.0f, 2.0f, 1.0f), XMMatrixTranslation(0.0f, 4.0f, -4.5f));
	m_worldCont[4] = XMMatrixMultiply(XMMatrixScaling(4.0f, 2.0f, 1.0f), XMMatrixTranslation(0.0f, 4.0f, -9.5f));
	m_worldWater = XMMatrixMultiply(XMMatrixScaling(0.2f, 1.0f, 0.2f), XMMatrixTranslation(0.0f, 4.5f, -7.0f));

	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------
	if (!m_shapes.Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_rendererShiny.Initialize(m_d3dDevice, m_hWnd, m_matShiny))
	{
		return false;
	}

	if (!m_rendererRough.Initialize(m_d3dDevice, m_hWnd, m_matRough))
	{
		return false;
	}

	if (!m_rendererWater.Initialize(m_d3dDevice, m_hWnd, m_matWater))
	{
		return false;
	}

	return true;
}

bool SimpleApp::Init()
{
	//Engine Init
	if (!BaseApp::Init())
	{
		return false;
	}

	//Additional init
	if (!SceneInit())
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
	
	m_waterX += 0.05f * dt;
	m_waterY += 0.05f * dt;
	XMMATRIX grassOffset = XMMatrixTranslation(m_waterX, m_waterY, 0.0f);
	XMMATRIX grassScale = XMMatrixScaling(2.0f, 2.0f, 0.0f);
	m_texTransfWater = XMMatrixMultiply(grassScale, grassOffset);
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

	//----------Rendering----------------------------------------------------------------------
	m_rendererRough.Render(m_d3dDeviceContext, m_worldPlain, m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_texGrass, m_texTransfGrass, m_offsetPlain.indexCount, m_offsetPlain.indexOffset, m_offsetPlain.vertexOffset);

	for (int i = 0; i < 5; ++i)
	{
		m_rendererShiny.Render(m_d3dDeviceContext, m_worldCont[i], m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_texBrick, m_texTransfBrick, m_offsetCont[i].indexCount, m_offsetCont[i].indexOffset, m_offsetCont[i].vertexOffset);
	}

	m_rendererWater.Render(m_d3dDeviceContext, m_worldWater, m_view, m_projectionMatrix, m_basicLight, m_camera.GetPosition(), m_texWater, m_texTransfWater, m_offsetWater.indexCount, m_offsetWater.indexOffset, m_offsetWater.vertexOffset);

	//-----------------------------------------------------------------------------------------

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

		m_theta -= dx*2.2f;
		m_radius += dy*2.2f;
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