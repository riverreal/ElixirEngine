#include "RadixScene.h"

Scene::Scene(Model* model, DirectX::XMMATRIX& projMatrix)
	:m_renderingMode(RENDER_MODE::DEFERRED_RENDERING),
	m_model(model),
	m_sceneReady(false)
{
	m_camera = new Camera();
	m_camera->SetPosition(0.0f, 3.0f, 0.0f);
	m_camera->SetProjection(projMatrix);

	m_lighting = new Light();

	m_fog.Enabled = true;
	m_fog.FogColor = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_fog.FogStart = 250.0f;
	m_fog.FogRange = 1000.0f;

	m_sky = new Object();
	m_sky->SetOffset(m_model->AddGeometry(MODEL_TYPE_SPHERE));
	m_sky->SetScale(30.0f, 30.0f, 30.0f);

	
}

Scene::~Scene()
{
	CleanMemory();
	SafeRelease(m_sky);
	SafeRelease(m_camera);
}

void Scene::AddObject(Object * obj)
{
	//RadixLog("Added Object");
	m_objChildren.push_back(obj);
}

void Scene::RemoveObject(Object* obj)
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

void Scene::UpdateScene()
{
	for (auto &object : m_objChildren)
	{
		if (object->GetDynamic())
		{
			object->Update();
		}
	}

	if (m_sky)
	{
		m_sky->SetPosition(m_camera->GetPosition());
	}
}

Camera * Scene::GetCamera()
{
	return m_camera;
}

std::vector<Object*> Scene::GetChildren()
{
	return m_objChildren;
}

Object * Scene::GetObjectByName(std::string name)
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
Object * Scene::GetObjectByName(std::string name, Object * parent)
{
	if (parent->GetName() == name)
	{
		return parent;
	}

	for (auto &object : parent->GetChildren())
	{
		auto result = GetObjectByName(name, object);

		if (result != nullptr)
		{
			return result;
		}

		/*
		auto objName = object->GetName();
		if (name == objName)
		{
			return object;
		}

		for (auto &child : object->GetChildren())
		{
			auto result = GetObjectByName(name, child);

			if (result != nullptr)
			{
				return result;
			}
		}
		*/
	}

	return nullptr;
}

Object * Scene::GetObjectByTag(unsigned int tag)
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

void Scene::SetEnvMap(ID3D11ShaderResourceView * envMap)
{
	m_envMap = envMap;

	//Sky Object will only need albedo texture to work as a skydome.
	m_sky->SetTexture(m_envMap, TEXTURE_TYPE::ALBEDO);
}

ID3D11ShaderResourceView * Scene::GetEnvMap()
{
	return m_envMap;
}

void Scene::SetIrradiance(ID3D11ShaderResourceView * irra)
{
	m_irradiance = irra;
}

ID3D11ShaderResourceView * Scene::GetIrradiance()
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
}

bool Scene::IsSceneReady()
{
	return m_sceneReady;
}

Object * Scene::GetSky()
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
