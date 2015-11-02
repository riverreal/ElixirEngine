#include "TextureManager.h"
#include "../Helper/GeneralHelper.h"

using namespace std;

TextureManager::TextureManager()
	:m_totalInstantCacheID(0),
	m_totalOpenCacheID(0)
{

}

TextureManager::~TextureManager()
{
	for (UINT i = 0; i < m_instantCache.size(); ++i)
	{
		ReleaseCOM(m_instantCache[i]);
	}
	for (UINT i = 0; i < m_openCache.size(); ++i)
	{
		ReleaseCOM(m_openCache[i]);
	}
}

int TextureManager::StoreTextureInstantCache(ID3D11Device* device, wstring filename)
{
	int instantID = m_totalInstantCacheID;
	
	m_instantCache.push_back(TextureLoader::CreateDDSTexture(device, filename.c_str()));
	if (m_instantCache[m_totalInstantCacheID] == nullptr)
	{
		m_instantCache.push_back(TextureLoader::CreateWICTexture(device, filename.c_str()));
		if (m_instantCache[m_totalInstantCacheID] == nullptr)
		{
			instantID = 0;
			MessageBox(0, L"Cant Store Texture in Instant Cache!", 0, 0);
			return instantID;
		}
	}

	m_totalInstantCacheID++;

	return instantID;
}

int TextureManager::StoreTextureOpenCache(ID3D11Device* device, wstring filename)
{
	int openID;

	return openID;
}

ID3D11ShaderResourceView* TextureManager::GetTextureFromInstant(UINT textureID)
{
	ID3D11ShaderResourceView* texture;
	if (m_instantCache[textureID] == nullptr)
	{
		MessageBox(0, L"Can't get texture by id", 0, 0);
		texture = nullptr;
	}
	else
	{
		texture = m_instantCache[textureID];
	}

	return texture;
}

ID3D11ShaderResourceView* TextureManager::GetTextureFromOpen(UINT textureID)
{
	ID3D11ShaderResourceView* texture;

	return texture;
}

void TextureManager::ReleaseTextureFromInstant(UINT textureID)
{

}

void TextureManager::ReleaseTextureFromOpen(UINT textureID)
{

}