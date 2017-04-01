#include "PulseGame.h"
#include "../Source/System/GameManager.h"
#include "../Source/Includes/LESystem.h"

PulseGame::PulseGame()
{
}

/*
void PulseGame::StartScene()
{
	m_sceneManager = GameManager::GetInstance().GetSceneManager();
	m_scene = m_sceneManager->CreateScene("TestScene");
	m_scene->SetInitFunction([this]() {this->Init(); });
	m_scene->SetUpdateFunction([this](float dt) {this->Update(dt);});
}
*/

void PulseGame::InitTestScene()
{	
	ThisScene->SetIrradiance(Manager->GetTextureManager()->AddTexture(L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	ThisScene->SetEnvMap(Manager->GetTextureManager()->AddTexture(L"Resources/Textures/Cubemaps/dayCube.dds"));

	auto dirL = ThisScene->GetLight()->GetModDirectionalLight();

	dirL->LightColor[0] = 1.0f;
	dirL->LightColor[1] = 0.76f;
	dirL->LightColor[2] = 0.21f;

	dirL->LightIntensity[0] = 3.2f;
	dirL->LightIntensity[1] = 0.1f;

	dirL->Direction[0] = 0.1f;
	dirL->Direction[1] = -0.6f;
	dirL->Direction[2] = -0.8f;

	ThisScene->GetCamera()->SetPosition(0.0f, 0.0f, -10.0f);

	ElixirLog("TestScene Initialized");
}

void PulseGame::UpdateTestScene(float dt)
{
}

void PulseGame::Init()
{
	InitTestScene();
}

void PulseGame::Update(float dt)
{
	auto cube = ThisScene->GetObjectByName("Cube");
	if (cube)
	{
		cube->GetTransform()->Rotation.y += 10.0f * dt;
	}
}
