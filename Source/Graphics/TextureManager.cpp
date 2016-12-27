#include "TextureManager.h"
#include "../Helper/GeneralHelper.h"

using namespace std;

TextureManager::TextureManager()
	:m_idCounter(0)
{
	//Reserve ID 0
	m_textureVector.push_back(nullptr);
	m_idCounter++;
}

TextureManager::~TextureManager()
{
	
}

U32 TextureManager::AddTexture(ID3D11Device * device, std::wstring filepath)
{
	auto id = m_idCounter;
	auto textureItem = new TextureItem();
	textureItem->srv = TextureLoader::CreateTexture(device, filepath.c_str());
	textureItem->name = filepath;
	m_textureVector.push_back(textureItem);
	m_idCounter++;

	return id;
}

U32 TextureManager::AddTexture(ID3D11Device * device, std::wstring filepath, std::wstring name)
{
	auto id = m_idCounter;
	auto textureItem = new TextureItem();
	textureItem->srv = TextureLoader::CreateTexture(device, filepath.c_str());
	textureItem->name = name;
	m_textureVector.push_back(textureItem);
	m_idCounter++;

	return id;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(std::wstring name)
{
	for (auto &textureItem : m_textureVector)
	{
		if (textureItem->name == name)
		{
			return textureItem->srv;
		}
	}

	ElixirLog("TextureManager: Couldn't find texture at GetTexture(" + ws2s(name) + ")");
	return nullptr;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(U32 id)
{
	if (id <= m_idCounter)
	{
		return m_textureVector[id]->srv;
	}

	ElixirLog("TextureManager: ID provided index outof range.");

	return nullptr;
}


U32 TextureManager::GetID(std::wstring name)
{
	U32 indexCounter = 0;
	for (auto &textureItem : m_textureVector)
	{
		if (textureItem->name == name)
		{
			return indexCounter;
		}
		indexCounter++;
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
		}
		else
		{
			reservedPassed = true;
		}
	}

	m_textureVector.clear();
}
