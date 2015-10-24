#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"

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
	ID3D11ShaderResourceView* m_texRough;
	ID3D11ShaderResourceView* m_texMirror;
	ID3D11ShaderResourceView* m_texNull;
	DirectX::XMMATRIX m_texTranfDef;
	DirectX::XMMATRIX m_texTranfCrate;
	DirectX::XMMATRIX m_texTransfGrass;
	DirectX::XMMATRIX m_texTransfWater;
	DirectX::XMMATRIX m_texTransfBrick;

	//Light
	BasicLight m_basicLight;

	//Fog
	Fog m_fog;

	//Materials
	Material m_matShiny;
	Material m_matRough;
	Material m_matWater;
	Material m_matShadow;

	//Renderers
	LightShader m_rendererLightShader;

	//Objects
	Object m_plane;
	DirectX::XMFLOAT3 m_eyePos;

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
	:BaseApp(instance, width, height)
{
}

SimpleApp::~SimpleApp()
{
	m_rendererLightShader.Shutdown();
	m_shapes.Shutdown();
	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	//Vertor based Variables

	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	m_texTranfDef = XMMatrixIdentity();
	m_texTransfGrass = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(70.0f, 70.0f, 1.0f));
	m_texTranfCrate = m_texTranfDef;
	m_texTransfBrick = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	m_texTransfWater = XMMatrixMultiply(m_texTranfDef, XMMatrixScaling(1.0f, 1.0f, 1.0f));

	m_texCrate = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/WoodCrate01.dds");
	m_texWater = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/water2.dds");
	m_texGrass = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/grass.dds");
	m_texBrick = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/darkbrickdxt1.dds");
	m_texRough = TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/semi-rough.jpg");
	m_texMirror = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/ice.dds");
	m_texNull = TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/water1.dds");

	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------
	m_basicLight.Directional.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_basicLight.Directional.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_basicLight.Directional.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
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
	//        Fog Init
	//-----------------------------------------------------------------------------------------------------
	m_fog.Enabled = true;
	m_fog.FogColor = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_fog.FogStart = 15.0f;
	m_fog.FogRange = 175.0f;

	//-----------------------------------------------------------------------------------------------------
	//        Material Init
	//-----------------------------------------------------------------------------------------------------
	m_matShiny.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_matShiny.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_matShiny.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 2.5f);

	m_matRough.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_matRough.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_matRough.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_matWater.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_matWater.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f); //Semi Transparent water
	m_matWater.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	//-------Shadow material
	m_matShadow.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_matShadow.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.7f);
	m_matShadow.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------

	m_plane.SetOffset(m_shapes.AddGeometry(MODEL_TYPE_PLAIN));


	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------

	m_plane.SetScale(100.0f, 1.0f, 100.0f);
	m_plane.SetPosition(0.0f, 0.0f, 0.0f);


	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------
	if (!m_shapes.Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_rendererLightShader.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	m_camera.SetPosition(0.0f, 3.0f, 0.0f);

	return true;
}

bool SimpleApp::Init()
{
	//Engine Init
	if (!BaseApp::Init())
	{
		return false;
	}

	//Enable blend state
	if (!BlendState::Init(m_d3dDevice))
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
	
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_camera.Walk(-10.0f*dt);
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_camera.Walk(10.0f*dt);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_camera.Strafe(-10.0f*dt);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_camera.Strafe(10.0f*dt);
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_camera.SetPosition(0.0f, 10.0f, 0.0f);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{	
	}
	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_fog.Enabled = true;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_fog.Enabled = false;
	}
	
	m_plane.Update();
}

void SimpleApp::Draw()
{
	assert(m_d3dDeviceContext);
	assert(m_swapChain);

	float color[4] = {0, 0, 0, 255};

	m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_camera.Update();

	XMMATRIX view;
	view = m_camera.GetViewMatrix();
	m_eyePos = m_camera.GetPosition();

	m_shapes.Render(m_d3dDeviceContext);

	//----------Rendering----------------------------------------------------------------------
	m_d3dDeviceContext->RSSetState(m_solidRS);
	m_rendererLightShader.Render(m_d3dDeviceContext, m_plane.GetWorldMatrix(), view, m_projectionMatrix, m_basicLight, m_fog, m_eyePos, m_texGrass, m_texTransfGrass, m_plane.GetOffset(), m_matRough);
	
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
		float dx = XMConvertToRadians(0.15f*static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.15f*static_cast<float>(y - m_lastMousePos.y));

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}