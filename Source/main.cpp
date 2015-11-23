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
	TextureManager m_texureManager;
	//Light
	BasicLight m_basicLight;
	//Fog
	Fog m_fog;
	//Renderers
	LightShader m_rendererLightShader;
	SkyDome m_rendererSky;
	//Objects
	Object* m_plane;
	Object* m_sky;

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
	m_rendererSky.Shutdown();
	m_rendererLightShader.Shutdown();
	m_shapes.Shutdown();
	delete m_sky;
	m_sky = 0;
	delete m_plane;
	m_plane = 0;
	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	//Object Init
	m_plane = new Object;
	m_sky = new Object;
	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	m_plane->SetTexTransformScale(3.0f, 3.0f, 3.0f);
	m_plane->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/semi-rough.jpg"), 0);
	m_plane->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"), 1);
	m_plane->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/snowcube1024.dds"), 2);
	m_sky->SetTexTransformScale(10.0f, 10.0f, 1.0f);
	m_sky->SetTexture(m_plane->GetTexture(2), 0);
	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------
	m_basicLight.Directional.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_basicLight.Directional.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_basicLight.Directional.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_basicLight.Directional.Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
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
	m_fog.Enabled = false;
	m_fog.FogColor = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_fog.FogStart = 15.0f;
	m_fog.FogRange = 175.0f;
	//-----------------------------------------------------------------------------------------------------
	//        Material Init
	//-----------------------------------------------------------------------------------------------------
	m_plane->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_plane->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_plane->SetMaterialSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 80.0f));
	m_plane->SetMaterialProperties(XMFLOAT4(1.0f, 0.0f, 0.76f, 1.0f));
	m_sky->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_sky->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_sky->SetMaterialSpecular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------
	m_plane->SetOffset(m_shapes.AddCustomGeometry(L"Resources/Models/car.txt"));
	//m_plane->SetOffset(m_shapes.AddGeometry(MODEL_TYPE_SPHERE));
	m_sky->SetOffset(m_shapes.AddGeometry(MODEL_TYPE_SPHERE));
	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------
	m_plane->SetScale(1.0f, 1.0f, 1.0f);
	m_plane->SetPosition(0.0f, 3.0f, 5.0f);
	m_sky->SetPosition(0.0f, 0.0f, 0.0f);
	m_sky->SetScale(30.0f, 30.0f, 30.0f);
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

	if (!m_rendererSky.Initialize(m_d3dDevice, m_hWnd))
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

	//dynamic object
	//m_plane->SetPosition(m_camera.GetPosition());
	m_plane->Update();

	m_sky->SetPosition(m_camera.GetPosition());
	m_sky->Update();
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
	m_rendererLightShader.Render(m_d3dDeviceContext, m_plane, m_camera, m_projectionMatrix, m_basicLight, m_fog);
	
	m_d3dDeviceContext->RSSetState(m_solidNoCullRS);
	m_d3dDeviceContext->OMSetDepthStencilState(m_skyDSS, 1);
	m_rendererSky.Render(m_d3dDeviceContext, m_sky, m_camera, m_projectionMatrix, m_basicLight, m_fog);
	m_d3dDeviceContext->RSSetState(m_solidRS);
	m_d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

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