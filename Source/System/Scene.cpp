#include "Scene.h"
#include "Components/ComponentIncludes.h"
#include "Systems/SystemIncludes.h"

using namespace Elixir;

Scene::Scene(Model* model, DirectX::XMMATRIX& projMatrix)
	:m_renderingMode(RENDER_MODE::DEFERRED_RENDERING),
	m_model(model),
	m_sceneReady(false),
	m_name("NewScene")
{
	m_camera = new Camera();
	m_camera->SetPosition(0.0f, 3.0f, 0.0f);
	m_camera->SetProjection(projMatrix);

	m_lighting = new Light();

	m_fog.Enabled = true;
	m_fog.FogColor = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_fog.FogStart = 250.0f;
	m_fog.FogRange = 1000.0f;

	m_sky = new GameObject();
	auto trans = m_sky->AddComponent<Transform>();
	auto renderer = m_sky->AddComponent<Renderer3D>();
	m_sky->GetRenderer()->Model = m_model->AddGeometry(MODEL_TYPE_SPHERE);
	m_sky->GetTransform()->Scale = Vec3f(30.0f);

	//Add default systems
	AddSystem<TransformSystem>();
	//AddSystem<IScriptSystem>();
}

Scene::~Scene()
{
	CleanMemory();
	SafeRelease(m_sky);
	SafeRelease(m_camera);
}

void Scene::SetName(std::string name)
{
	m_name = name;
}

std::string Scene::GetName()
{
	return m_name;
}

void Scene::AddObject(GameObject * obj)
{
	//RadixLog("Added Object");
	m_objChildren.push_back(obj);
}

void Scene::RemoveObject(GameObject* obj)
{
	auto objectToRemove = std::find(m_objChildren.begin(), m_objChildren.end(), obj);

	if (objectToRemove == m_objChildren.end())
	{
		//Could not find the object
		RadixLog("Could not find object.");
	}
	else
	{
		auto index = std::distance(m_objChildren.begin(), objectToRemove);
		CleanObjFromMemory(index);
		m_objChildren.erase(objectToRemove);
	}
}

void Scene::RemoveObjectByName(std::string name)
{
	auto object = GetObjectByName(name);
	if (object != nullptr)
	{
		RemoveObject(object);
	}
}

void Scene::RemoveObjectByTag(unsigned int tag)
{
	auto object = GetObjectByTag(tag);
	if (object != nullptr)
	{
		RemoveObject(object);
	}
}

void Scene::RemoveAllObjects()
{
	CleanMemory();
	m_objChildren.clear();
}

void Scene::SetRenderMode(unsigned int mode)
{
	m_renderingMode = mode;
}

void Scene::UpdateScene(F32 deltaTime)
{
	for (auto &system : m_systems)
	{
		for (auto &object : m_objChildren)
		{
			system.second->Update(object, deltaTime);

			UpdateSceneRec(object, system.second, deltaTime);
		}
	}

	if (m_sky)
	{
		m_sky->GetTransform()->Position = m_camera->GetPosition();
		
		auto tSystem = GetSystem<TransformSystem>();
		tSystem->Update(m_sky, deltaTime);
		
	}
}

Camera * Scene::GetCamera()
{
	return m_camera;
}

std::vector<GameObject*> Scene::GetChildren()
{
	return m_objChildren;
}

GameObject * Scene::GetObjectByName(std::string name)
{
	for (auto &object : m_objChildren)
	{
		auto result = GetObjectByName(name, object);
		if (result != nullptr)
		{
			return result;
		}
	}

	RadixLog("Could not find object.");

	return nullptr;
}

//loop through children
GameObject * Scene::GetObjectByName(std::string name, GameObject * parent)
{
	if (parent->GetName() == name)
	{
		return parent;
	}

	for (auto &object : parent->GetChildren())
	{
		//Recursive call
		auto result = GetObjectByName(name, object);

		if (result != nullptr)
		{
			return result;
		}
	}

	return nullptr;
}

GameObject * Scene::GetObjectByTag(unsigned int tag)
{
	//bool found = false;
	for (auto &object : m_objChildren)
	{
		auto objTag = object->GetTag();
		if (tag == objTag)
		{
			//found = true;
			return object;
		}
	}

	//if (!found)
	{
		//Could not find the object
		RadixLog("Could not find object.");
	}

	return nullptr;
}

Model * Scene::GetModel()
{
	return m_model;
}

unsigned int Scene::GetRenderMode()
{
	return m_renderingMode;
}

void Scene::SetEnvMap(U32 envMap)
{
	m_envMap = envMap;

	//Sky Object will only need albedo texture to work as a skydome.
	m_sky->GetRenderer()->Material.albedo = m_envMap;
}

U32 Scene::GetEnvMap()
{
	return m_envMap;
}

void Scene::SetIrradiance(U32 irra)
{
	m_irradiance = irra;
}

U32 Scene::GetIrradiance()
{
	return m_irradiance;
}

void Scene::SetLight(Light * light)
{
	m_lighting = light;
}

Light * Scene::GetLight()
{
	return m_lighting;
}

Fog Scene::GetFog()
{
	return m_fog;
}

void Scene::SceneReady()
{
	m_sceneReady = true;

	for (auto &system : m_systems)
	{
		for (auto &object : m_objChildren)
		{
			system.second->Initialize(object);

			SceneReadyRec(object, system.second);
		}
	}

	if (m_sky)
	{
		auto tSystem = GetSystem<TransformSystem>();
		tSystem->Initialize(m_sky);
	}
}

bool Scene::IsSceneReady()
{
	return m_sceneReady;
}

GameObject * Scene::GetSky()
{
	return m_sky;
}

void Scene::CleanMemory()
{
	//Delete every children from memory
	for (auto &object : m_objChildren)
	{
		SafeRelease(object);
	}
}

void Scene::CleanObjFromMemory(unsigned int index)
{
	SafeRelease(m_objChildren[index]);
}

void Scene::SceneReadyRec(GameObject* parent, IComponentSystem* system)
{
	for (auto &object : parent->GetChildren())
	{
		system->Initialize(object);

		SceneReadyRec(object, system);
	}
}

void Scene::UpdateSceneRec(GameObject* parent, IComponentSystem* system, F32 deltaTime)
{
	for (auto &object : parent->GetChildren())
	{
		system->Update(object, deltaTime);

		UpdateSceneRec(object, system, deltaTime);
	}
}

template <typename T>
T* Scene::AddSystem()
{
	IComponentSystem* system = new T;

	m_systems[&typeid(*system)] = system;

	return static_cast<T*>(system);
}

template <typename T>
T* Scene::GetSystem()
{
	if (m_systems.count(&typeid(T)) != 0)
	{
		return static_cast<T*>(m_systems[&typeid(T)]);
	}
	else
	{
		ElixirLog("Could not get system");
		return nullptr;
	}
}

GameObject* Scene::CreateObject()
{
	auto object = new GameObject();
	m_objChildren.push_back(object);

	return object;
}

GameObject* Scene::CreateObject(OBJECT_PRESET objPreset)
{
	auto object = new GameObject();
	m_objChildren.push_back(object);

	switch (objPreset)
	{

	case OBJECT_TRANSFORM:
		object->AddComponent<Transform>();
		break;

	case OBJECT_RENDER:
		object->AddComponent<Transform>();
		object->AddComponent<Renderer3D>();
		break;

	case OBJECT_EMPTY:
	default:
		break;

	}

	return object;
}