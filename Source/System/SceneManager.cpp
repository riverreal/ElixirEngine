#include "SceneManager.h"

Elixir::SceneManager::SceneManager()
	:m_currentSceneName("")
{
}

Elixir::SceneManager::~SceneManager()
{
	for (auto &scene : m_scenes)
	{
		SafeRelease(scene);
	}

	m_scenes.clear();
}

Elixir::Scene* Elixir::SceneManager::CreateScene(std::string name, Model * model, DirectX::XMMATRIX & projMatrix)
{
	auto scene = new Scene(model, projMatrix);
	scene->SetName(name);
	AddScene(scene);

	return scene;
}

void Elixir::SceneManager::AddScene(Scene * scene)
{
	m_scenes.push_back(scene);
	//Set this scene to current scene if it is the first scene ever added.
	if (m_currentSceneName == "")
	{
		m_currentSceneName = scene->GetName();
	}
}

Elixir::Scene* Elixir::SceneManager::GetScene(std::string name)
{
	for (auto &scene : m_scenes)
	{
		if (scene->GetName() == name)
		{
			return scene;
		}
	}

	ElixirLog("Could not find Scene with name: " + name);

	return nullptr;
}

void Elixir::SceneManager::ChangeScene(std::string name)
{
	auto scene = GetScene(name);

	if(scene != nullptr)
		m_currentSceneName = scene->GetName();
}

Elixir::Scene* Elixir::SceneManager::GetCurrentScene()
{
	return GetScene(m_currentSceneName);
}
