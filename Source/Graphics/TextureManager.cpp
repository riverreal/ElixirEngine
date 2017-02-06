#include "TextureManager.h"
#include "../Helper/GeneralHelper.h"

using namespace std;

TextureManager::TextureManager(ID3D11Device* device)
	:m_idCounter(0),
	m_editorIDCounter(0)
{
	m_device = device;
	//Reserve ID 0
	m_textureVector.push_back(nullptr);
	m_idCounter++;

	AddTexture(L"Elixir/Default/albedo.png"); // id 1
	AddTexture(L"Elixir/Default/normal.jpg"); // id 2
	AddTexture(L"Elixir/Default/roughness.png"); // id 3
	AddTexture(L"Elixir/Default/metallic.png"); // id 4

	//other texture id starts from 5
	m_firstID = 4;

}

TextureManager::~TextureManager()
{
}

U32 TextureManager::AddTexture(std::wstring filepath)
{
	if (GetTexture(filepath) == nullptr)
	{
		auto id = m_idCounter;
		auto textureItem = new TextureItem();
		textureItem->srv = TextureLoader::CreateTexture(m_device, filepath.c_str());
		textureItem->name = filepath;
		m_textureVector.push_back(textureItem);
		m_idCounter++;
		return id;
	}
	else
	{
		return GetID(filepath);
	}
	
}

U32 TextureManager::AddEditorTexture(std::wstring filepath)
{
	m_firstID++;

	if (GetTexture(filepath) == nullptr)
	{
		auto id = m_idCounter;
		auto textureItem = new TextureItem();
		textureItem->srv = TextureLoader::CreateTexture(m_device, filepath.c_str());
		textureItem->name = filepath;
		m_textureVector.insert(m_textureVector.begin() + m_firstID, textureItem);
		m_editorIDCounter++;
		m_idCounter++;
		return m_firstID;
	}
	else
	{
		return GetID(filepath);
	}
}

U32 TextureManager::AddTexture(std::wstring filepath, std::wstring name)
{
	auto id = m_idCounter;
	auto textureItem = new TextureItem();
	textureItem->srv = TextureLoader::CreateTexture(m_device, filepath.c_str());
	textureItem->name = name;
	m_textureVector.push_back(textureItem);
	m_idCounter++;

	return id;
}

void TextureManager::AddAllocatedTexture(std::wstring filepath, U32 ID)
{
	m_textureVector[ID]->srv = TextureLoader::CreateTexture(m_device, filepath.c_str());
	m_textureVector[ID]->name = filepath;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(std::wstring name)
{
	for (auto &textureItem : m_textureVector)
	{
		if (textureItem)
		{
			if (textureItem->name == name)
			{
				return textureItem->srv;
			}
		}
	}

	//ElixirLog("TextureManager: Couldn't find texture at GetTexture(" + ws2s(name) + ")");
	return nullptr;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(U32 id)
{
	if (id <= m_idCounter)
	{
		return m_textureVector[id]->srv;
	}

	ElixirLog("TextureManager: ID provided index out of range.");

	return nullptr;
}

std::string TextureManager::GetTextureName(U32 id)
{
	if (id <= m_idCounter)
	{
		return ws2s(m_textureVector[id]->name);
	}

	return "";
}

void TextureManager::AllocateTexture()
{
	auto textureItem = new TextureItem();
	textureItem->srv = nullptr;
	textureItem->name = L"allocated";
	m_textureVector.push_back(textureItem);
	m_idCounter++;
}

U32 TextureManager::GetID(std::wstring name)
{
	U32 indexCounter = 0;
	for (auto &textureItem : m_textureVector)
	{
		if (textureItem)
		{
			if (textureItem->name == name)
			{
				return indexCounter+1;
			}
			indexCounter++;
		}
	}
	auto f = __FILE__;
	std::string file = f;
	auto line = __LINE__;
	ElixirLog("TextureManager: Couldn't find texture id at - " + file + "," + std::to_string(__LINE__));
	
	return 0;
}

void TextureManager::RemoveTexture(U32 id)
{
	auto texture = GetTexture(id);
	if (texture == nullptr)
	{
		ElixirLog("TextureManager: Couldn't RemoveTexture");
		return;
	}
	m_textureVector.erase(m_textureVector.begin() + id);
	ReleaseCOM(texture);
}

void TextureManager::Shutdown()
{
	bool reservedPassed = false;
	for (auto &textureItem : m_textureVector)
	{
		if (reservedPassed == true)
		{
			auto texture = textureItem->srv;
			ReleaseCOM(texture);
			SafeRelease(textureItem);
		}
		else
		{
			reservedPassed = true;
		}
	}

	m_textureVector.clear();
}

ID3D11Device * TextureManager::GetDevice()
{
	return m_device;
}

U32 TextureManager::GetMaxID()
{
	return m_idCounter;
}

U32 TextureManager::GetFirstID()
{
	return m_firstID + 1;
}
