#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"
#include <stdlib.h>
#include <time.h>
#include "../GameSource/PulseGame.h"

//16:9 resolutions
//480p - WVGA (854 x 480)
//720p (1280 x 720)
//900p (1600 x 900)
//1080p (1920 x 1080)

namespace Elixir
{
	const static int SCREEN_WIDTH = 1600;
	const static int SCRENN_HEIGHT = 900;
};

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
	Camera m_smoothCamera;

	PulseGame pulseGame;

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
	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	/*
	AudioManager::GetInstance().AddControlledMusic("Resource/Enthusiast.mp3");
	AudioManager::GetInstance().GetControlledMusic()->setIsLooped(true);

	m_isRightClick = false;
	m_scene = m_sceneManager->CreateScene("TestScene");
	m_speedMult = 1.0f;

	m_scene->SetIrradiance(m_textureManager->AddTexture(L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(m_textureManager->AddTexture(L"Resources/Textures/Cubemaps/earth_moon_skybox.dds"));

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

	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------
	*/
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

	//Early initialization of singleton
	AudioManager::GetInstance();

	pulseGame.StartScene("Pulse");

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
			AudioManager::GetInstance().GetControlledMusic()->setIsPaused(false);
		}

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			AudioManager::GetInstance().GetControlledMusic()->setIsPaused(true);
			m_speedMult = 4.0f;
		}
		else
		{
			m_speedMult = 1.0f;
		}
	}

	m_sceneManager->GetCurrentScene()->GetCamera()->SetPosition(MathHelper::lerp(m_sceneManager->GetCurrentScene()->GetCamera()->GetPosition(), m_smoothCamera.GetPosition(), 0.3f));
	m_sceneManager->GetCurrentScene()->GetCamera()->SetLook(MathHelper::lerp(m_sceneManager->GetCurrentScene()->GetCamera()->GetLook(), m_smoothCamera.GetLook(), 0.3f));
	m_sceneManager->GetCurrentScene()->GetCamera()->SetRight(MathHelper::lerp(m_sceneManager->GetCurrentScene()->GetCamera()->GetRight(), m_smoothCamera.GetRight(), 0.3f));
	m_sceneManager->GetCurrentScene()->GetCamera()->SetUp(MathHelper::lerp(m_sceneManager->GetCurrentScene()->GetCamera()->GetUp(), m_smoothCamera.GetUp(), 0.3f));

#endif

	m_sceneManager->UpdateCurrentScene(dt);
}

void SimpleApp::OnMouseDown(WPARAM btnState, int x, int y)
{
#if ELIXIR_EDITOR == true
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;

	SetCapture(m_hWnd);
#endif
}

void SimpleApp::OnMouseUp(WPARAM btnState, int x, int y)
{
#if ELIXIR_EDITOR == true
	ReleaseCapture();
	//if ((btnState & MK_RBUTTON) != 0)
		m_isRightClick = false;
#endif
}

void SimpleApp::OnMouseMove(WPARAM btnState, int x, int y)
{
#if ELIXIR_EDITOR == true
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

#endif
}
