#pragma once

#include <vector>
#include "Object.h"
#include <iostream>
#include "../Helper/GeneralHelper.h"
#include "../Graphics/CameraManager.h"
#include "../Graphics/ModelManager.h"
#include "../Graphics/Light.h"

class Scene
{
public:
	Scene(Model* model, DirectX::XMMATRIX& projMatrix);
	~Scene();

	//Adds an object to the scene
	void AddObject(Object* obj);

	//Removes an object from the scene
	//WARNING: Also removes from memory
	void RemoveObject(Object* obj);

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
	void UpdateScene();

	//Returns the camera used in the scene
	Camera* GetCamera();

	//Returns the vector of objects
	std::vector<Object*> GetChildren();

	//Returns an object by name
	Object* GetObjectByName(std::string name);
	

	//Returns an object by tag
	Object* GetObjectByTag(unsigned int tag);

	//Returns the ModelManager
	Model* GetModel();

	//Returns the render mode
	unsigned int GetRenderMode();

	//Sets the environment map
	void SetEnvMap(ID3D11ShaderResourceView* envMap);

	//Returns the environment map
	ID3D11ShaderResourceView* GetEnvMap();

	//Sets the irradiance
	void SetIrradiance(ID3D11ShaderResourceView* irra);

	//Returns the irradiance
	ID3D11ShaderResourceView* GetIrradiance();

	//Sets the light for the scene
	void SetLight(Light* light);

	//Returns the light used in the scene
	Light* GetLight();

	Fog GetFog();

	//Set ready
	void SceneReady();

	bool IsSceneReady();

	Object* GetSky();

	
	
private:
	//Used recursively to loop through parent object's children too
	Object* GetObjectByName(std::string name, Object* parent);

	//Removes every children from memory
	void CleanMemory();
	//Removes a single child from memory
	void CleanObjFromMemory(unsigned int index);

private:
	//A vector of all objects added to the scene
	std::vector<Object*> m_objChildren;
	Object* m_sky;
	Camera* m_camera;
	Model* m_model;
	//Deferred(Default) or Foreward
	unsigned int m_renderingMode;

	ID3D11ShaderResourceView* m_envMap;
	ID3D11ShaderResourceView* m_irradiance;
	Light* m_lighting;
	Fog m_fog;
	bool m_sceneReady;
};