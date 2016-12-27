#pragma once

#include "Scene.h"

namespace Elixir
{
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		//Create a new scene and add it to the SceneManager.
		//New created scene will be owned by SceneManager.
		Scene* CreateScene(std::string name, Model* model, DirectX::XMMATRIX& projMatrix);
		void AddScene(Scene* scene);
		Scene* GetScene(std::string name);
		void ChangeScene(std::string name);
		Scene* GetCurrentScene();

	private:


	private:
		std::vector<Scene*> m_scenes;
		std::string m_currentSceneName;

	};
}