#include "IElixirGame.h"

#include "GameManager.h"
#include "SceneManager.h"

void IElixirGame::StartScene(std::string sceneName)
{
	Manager = Elixir::GameManager::GetInstance().GetSceneManager();
	ThisScene = Manager->CreateScene(sceneName);
	ThisScene->SetInitFunction([this]() {this->Init(); });
	ThisScene->SetUpdateFunction([this](float dt) {this->Update(dt); });
}
