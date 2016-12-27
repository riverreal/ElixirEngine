#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"
#include <stdlib.h>
#include <time.h>

//16:9 resolutions
//480p - WVGA (854 x 480)
//720p (1280 x 720)
//900p (1600 x 900)
//1080p (1920 x 1080)

const int SCREEN_WIDTH = 1600;
const int SCRENN_HEIGHT = 900;

using namespace Elixir;

class SimpleApp : public BaseApp
{
public:
	SimpleApp(HINSTANCE instance, int width, int height);
	~SimpleApp();

	bool Init() override;
	void Update(float dt) override;
	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	bool SceneInit();

private:
	Model* m_shapes;
	TextureManager m_texureManager;

	Camera m_smoothCamera;

	//Shader for sky
	SkyDome m_rendererSky;

	Scene* m_scene;
	float m_speedMult;

	POINT m_lastMousePos;
	bool m_isRightClick;
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
	m_shapes->Shutdown();

	SafeRelease(m_shapes);

	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	m_isRightClick = false;
	m_shapes = new Model();
	m_scene = m_sceneManager->CreateScene("TestScene", m_shapes, m_projectionMatrix);
	m_speedMult = 1.0f;

	m_scene->SetIrradiance(m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/dayCube.dds"));

	auto dirL = m_scene->GetLight()->GetModDirectionalLight();

	dirL->LightColor[0] = 1.0f;
	dirL->LightColor[1] = 0.76f;
	dirL->LightColor[2] = 0.21f;

	dirL->LightIntensity[0] = 3.2f;
	dirL->LightIntensity[1] = 0.1f;

	dirL->Direction[0] = 0.1f;
	dirL->Direction[1] = -0.6f;
	dirL->Direction[2] = -0.8f;

	m_scene->GetCamera()->SetPosition(0.0f, 0.0f, 0.0f);
	m_smoothCamera.SetPosition(0.0f, 0.0f, 0.0f);

	//Load textures
	auto albedo = m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/material/blocks/albedo.png", L"ForestAlbedo");
	auto normal = m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/material/blocks/normal512.jpg", L"ForestNormal");
	auto roughness = m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/balls/175.png", L"NoValue");
	auto metallic = m_textureManager->AddTexture(m_d3dDevice, L"Resources/Textures/balls/0.png", L"SemiValue");

	//Create material for the forest ground
	Material forestGroundMat(albedo, normal, roughness, metallic);

	//Create GameObject of the forest ground
	auto forestGround = m_scene->CreateObject(OBJECT_RENDER);
	forestGround->SetName("forestG");
	forestGround->GetRenderer()->Material = forestGroundMat;
	forestGround->GetRenderer()->Model = m_shapes->AddGeometry(MODEL_TYPE_SPHERE);
	forestGround->GetTransform()->Position.y = 1.0f;
	
	
	auto forestBox = m_scene->CreateObject(OBJECT_RENDER);
	forestBox->SetName("forestBox");
	forestBox->GetRenderer()->Material = forestGroundMat;
	forestBox->GetRenderer()->Model = m_shapes->AddGeometry(MODEL_TYPE_CUBE);


	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------

	if (!m_shapes->Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_rendererSky.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	m_scene->SceneReady();
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
#if ELIXIR_EDITOR == true
	if (m_isRightClick)
	{
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_smoothCamera.Walk(-10.0f*dt * m_speedMult);
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_smoothCamera.Walk(10.0f*dt* m_speedMult);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_smoothCamera.Strafe(-10.0f*dt * m_speedMult);
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_smoothCamera.Strafe(10.0f*dt * m_speedMult);
		}

		if (GetAsyncKeyState('R') & 0x8000)
		{
			m_smoothCamera.SetPosition(0.0f, 0.0f, 0.0f);
		}

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_speedMult = 4.0f;
		}
		else
		{
			m_speedMult = 1.0f;
		}
	}
#endif

	m_scene->GetCamera()->SetPosition(MathHelper::lerp(m_scene->GetCamera()->GetPosition(), m_smoothCamera.GetPosition(), 0.3f));
	m_scene->GetCamera()->SetLook(MathHelper::lerp(m_scene->GetCamera()->GetLook(), m_smoothCamera.GetLook(), 0.3f));
	m_scene->GetCamera()->SetRight(MathHelper::lerp(m_scene->GetCamera()->GetRight(), m_smoothCamera.GetRight(), 0.3f));
	m_scene->GetCamera()->SetUp(MathHelper::lerp(m_scene->GetCamera()->GetUp(), m_smoothCamera.GetUp(), 0.3f));
	m_scene->UpdateScene(dt);
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
	//if ((btnState & MK_RBUTTON) != 0)
		m_isRightClick = false;
}

void SimpleApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		m_isRightClick = true;
		float dx = XMConvertToRadians(0.15f*static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.15f*static_cast<float>(y - m_lastMousePos.y));

		m_smoothCamera.Pitch(dy);
		m_smoothCamera.RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}
