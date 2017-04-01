#pragma once

#include "Scene.h"
#include "FileIO.h"
#include "../Graphics/TextureManager.h"

namespace Elixir
{
	class SceneManager
	{
	public:
		SceneManager(TextureManager* tManager);
		~SceneManager();

		//Create a new scene and add it to the SceneManager.
		//New created scene will be owned by SceneManager.
		Scene* CreateScene(std::string name);
		void AddScene(Scene* scene);
		Scene* GetScene(std::string name);
		void ChangeScene(std::string name);
		Scene* GetCurrentScene();

		void AddProjectTextures();
		void AddNewTextures();
		void SaveProjectFile();

		void SaveCurrentScene();
		void RetrieveSceneData(GameObject* obj, SceneData& sData);
		//Loads a scene from a file (.escene)
		void LoadScene(std::string filename);

		Model* GetModel();
		TextureManager* GetTextureManager();
		FileIO* GetFileManager();

		void ResetModel();
		void ResetFileIO();

		void UpdateCurrentScene(float dt);

	private:


	private:
		bool m_sceneChanged;

		std::vector<Scene*> m_scenes;
		std::string m_currentSceneName;

		Model* m_model;
		TextureManager* m_textureManager;
		FileIO* m_fileManager;
	};
}