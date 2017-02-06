#include "SceneManager.h"
#include "Components\ComponentIncludes.h"
#include <algorithm>

Elixir::SceneManager::SceneManager(TextureManager* tManager)
	:m_currentSceneName("")
{
	m_fileManager = new FileIO();
	m_textureManager = tManager;
}

Elixir::SceneManager::~SceneManager()
{
	for (auto &scene : m_scenes)
	{
		SafeRelease(scene);
	}

	m_scenes.clear();

	SafeRelease(m_fileManager);
}

Elixir::Scene* Elixir::SceneManager::CreateScene(std::string name, Model * model, DirectX::XMMATRIX & projMatrix)
{
	m_model = model;
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

void Elixir::SceneManager::AddProjectTextures()
{
	auto projFilePath = m_fileManager->GetFilesPathInDirectory(".");
	bool foundProject = false;

	//Check if project file is available and load data if so.
	for (auto &filename : projFilePath)
	{
		auto extension = filename.substr(filename.find_last_of(".") + 1);
		if (extension == "elixir")
		{
			//load project
			auto project = m_fileManager->ReadProjFile(filename);

			for (auto &texData : project.TextureData)
			{
				//alocate in vector first
				m_textureManager->AllocateTexture();
			}

			for (auto &texData : project.TextureData)
			{
				//assign in correct ID
				m_textureManager->AddAllocatedTexture(s2ws(texData.name), texData.ID);
			}

			foundProject = true;
		}
	}

	//create new project if there is no existing project
	if (!foundProject)
	{
		m_fileManager->CreateProjFile();
	}

	AddNewTextures();
}

void Elixir::SceneManager::AddNewTextures()
{
	auto paths = m_fileManager->GetFilesPathInDirectory("Resource/");

	bool changed = false;

	for (auto &filename : paths)
	{
		auto extension = filename.substr(filename.find_last_of(".") + 1);
		//check file extension // if file type is image, add to manager
		if (extension == "jpg" || extension == "png" || extension == "dds" || extension == "bmp" || extension == "jpeg")
		{
			m_textureManager->AddTexture(s2ws(filename));
			changed = true;
		}
	}

	//save project file if there was a change
	if (changed)
	{
		SaveProjectFile();
	}
}

void Elixir::SceneManager::SaveProjectFile()
{
	ProjectData newProjData;
	for (U32 i = m_textureManager->GetFirstID(); i < m_textureManager->GetMaxID(); ++i)
	{
		StringID sid(m_textureManager->GetTextureName(i), i);

		newProjData.TextureData.push_back(sid);
	}

	m_fileManager->SaveProjFile(newProjData);
}

void Elixir::SceneManager::SaveCurrentScene()
{
	auto scene = GetCurrentScene();

	auto children = scene->GetChildren();

	SceneData newSceneData;

	newSceneData.name = scene->GetName();

	for (int i = 0; i < children.size(); ++i)
	{
		auto child = children[i];

		//Write SceneData recursively for each child
		RetrieveSceneData(child, newSceneData);
	}

	m_fileManager->SaveSceneFile(newSceneData);
}


void Elixir::SceneManager::RetrieveSceneData(GameObject * obj, SceneData& sData)
{
	//Create and set defaults
	ObjectID objData;
	//Defaults are already set in constructor but set to false again for safety
	objData.renderer.exists = false;
	objData.transform.exists = false;

	//Pack the Gameobject data to ObjectID struct
	objData.ID = 0;

	if (obj->GetTransform() != nullptr)
	{
		auto transform = obj->GetTransform();

		objData.transform.exists = true;

		objData.transform.dynamic = transform->Dynamic;
		objData.transform.pos = transform->Position;
		objData.transform.rot = transform->Rotation;
		objData.transform.sca = transform->Scale;
		objData.transform.tpos = transform->TexturePosition;
		objData.transform.trot = transform->TextureRotation;
		objData.transform.tsca = transform->TextureScale;
	}

	if (obj->GetRenderer() != nullptr)
	{
		auto renderer = obj->GetRenderer();

		objData.renderer.exists = true;

		objData.renderer.enabled = renderer->Enabled;
		objData.renderer.backface = renderer->EnableBackFaceCulling;
		objData.renderer.castShadow = renderer->CastShadow;
		objData.renderer.alb = renderer->Material.albedo;
		objData.renderer.norm = renderer->Material.normal;
		objData.renderer.rou = renderer->Material.roughness;
		objData.renderer.met = renderer->Material.metallic;
		objData.renderer.modelTypePrimitive = renderer->ModelTypePrimitive;
		objData.renderer.modelPath = renderer->ModelPath;
		objData.renderer.primitiveType = renderer->PrimitiveType;
	}

	sData.GameObjects.push_back(objData);

	for (auto &child : obj->GetChildren())
	{
		RetrieveSceneData(child, sData);
	}
}

void Elixir::SceneManager::LoadScene(std::string filename)
{
	auto sData = m_fileManager->ReadSceneFile(filename);
	
	auto currentScene = GetCurrentScene();
	currentScene->RemoveAllObjects();

	for (auto obj : sData.GameObjects)
	{
		auto newObject = currentScene->CreateObject();
		if (obj.transform.exists)
		{
			auto transform = newObject->AddComponent<Transform>();
			transform->Dynamic = obj.transform.dynamic;
			transform->Position = obj.transform.pos;
			transform->Rotation = obj.transform.rot;
			transform->Scale = obj.transform.sca;
			transform->TexturePosition = obj.transform.tpos;
			transform->TextureScale = obj.transform.tsca;
		}
		
		if (obj.renderer.exists)
		{
			auto render = newObject->AddComponent<Renderer3D>();

			render->Enabled = obj.renderer.enabled;
			render->EnableBackFaceCulling = obj.renderer.backface;
			render->CastShadow = obj.renderer.castShadow;
			render->ModelPath = obj.renderer.modelPath;
			render->ModelTypePrimitive = obj.renderer.modelTypePrimitive;
			render->Material.albedo = obj.renderer.alb;
			render->Material.normal = obj.renderer.norm;
			render->Material.roughness = obj.renderer.rou;
			render->Material.metallic = obj.renderer.met;
			render->PrimitiveType = obj.renderer.primitiveType;
			if (render->ModelTypePrimitive)
			{
				render->Model = m_model->AddGeometry(render->PrimitiveType);
			}
			else
			{
				render->Model = m_model->AddModelFromFile(obj.renderer.modelPath);
			}
			
			ResetModel();
		}
	}


}

Model * Elixir::SceneManager::GetModel()
{
	return m_model;
}

TextureManager * Elixir::SceneManager::GetTextureManager()
{
	return m_textureManager;
}

Elixir::FileIO * Elixir::SceneManager::GetFileManager()
{
	return m_fileManager;
}

void Elixir::SceneManager::ResetModel()
{
	m_model->Shutdown();
	m_model->Initialize(m_textureManager->GetDevice());
}

void Elixir::SceneManager::ResetFileIO()
{
	SafeRelease(m_fileManager);

	m_fileManager = new FileIO();

}
