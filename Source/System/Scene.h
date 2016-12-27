#pragma once

#include <vector>
#include "GameObject.h"
#include <iostream>
#include "../Helper/GeneralHelper.h"
#include "../Graphics/CameraManager.h"
#include "../Graphics/ModelManager.h"
#include "../Graphics/Light.h"
#include "../Interface/IComponentSystem.h"

namespace Elixir
{
	class Scene
	{
	public:
		Scene(Model* model, DirectX::XMMATRIX& projMatrix);
		~Scene();

		void SetName(std::string name);
		std::string GetName();

		//Adds an object to the scene
		void AddObject(GameObject* obj);

		//Removes an object from the scene
		//WARNING: Also removes from memory
		void RemoveObject(GameObject* obj);

		//Removes an object from the scene using its name
		//WARNING: Also removes from memory
		void RemoveObjectByName(std::string name);

		//Removes an object from the scene using its tag
		//WARNING: Also removes from memory
		void RemoveObjectByTag(unsigned int tag);

		//Removes every object from the scene
		//WARNING: Also removes from memory
		void RemoveAllObjects();

		//Sets a render mode using one of the RENDER_MODE available
		//Default render mode is Deferred Rendering
		void SetRenderMode(unsigned int mode);

		//Updates every dynamic object
		//Call this in Update function
		void UpdateScene(F32 deltaTime);

		//Returns the camera used in the scene
		Camera* GetCamera();

		//Returns the vector of objects
		std::vector<GameObject*> GetChildren();

		//Returns an object by name
		GameObject* GetObjectByName(std::string name);


		//Returns an object by tag
		GameObject* GetObjectByTag(unsigned int tag);

		//Returns the ModelManager
		Model* GetModel();

		//Returns the render mode
		unsigned int GetRenderMode();

		//Sets the environment map
		void SetEnvMap(U32 envMap);

		//Returns the environment map
		U32 GetEnvMap();

		//Sets the irradiance
		void SetIrradiance(U32 irra);

		//Returns the irradiance
		U32 GetIrradiance();

		//Sets the light for the scene
		void SetLight(Light* light);

		//Returns the light used in the scene
		Light* GetLight();

		Fog GetFog();

		//Set ready the scene
		//And Initialize Systems
		void SceneReady();

		bool IsSceneReady();

		GameObject* GetSky();

		template <typename T>
		T* AddSystem();

		template <typename T>
		T* GetSystem();

		GameObject* CreateObject();

		GameObject* CreateObject(OBJECT_PRESET objPreset);

	private:
		//Used recursively to loop through parent object's children too
		GameObject* GetObjectByName(std::string name, GameObject* parent);

		//Removes every children from memory
		void CleanMemory();
		//Removes a single child from memory
		void CleanObjFromMemory(unsigned int index);

		//Used recursively to loop through children of children
		//Initializes all systems
		void SceneReadyRec(GameObject* parent, IComponentSystem* system);

		//Used recursively to loop through children of children
		//Updates all systems
		void UpdateSceneRec(GameObject* parent, IComponentSystem* system, F32 deltaTime);

	private:
		//A vector of all objects added to the scene
		std::vector<GameObject*> m_objChildren;
		GameObject* m_sky;
		Camera* m_camera;
		Model* m_model;
		//Deferred(Default) or Foreward
		unsigned int m_renderingMode;

		std::string m_name;

		std::unordered_map<const std::type_info*, IComponentSystem*> m_systems;

		U32 m_envMap;
		U32 m_irradiance;
		Light* m_lighting;
		Fog m_fog;
		bool m_sceneReady;
	};
}

