#pragma once

#include "TextureLoader.h"


class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	int StoreTextureInstantCache(ID3D11Device* device, std::wstring fileName);
	int StoreTextureOpenCache(ID3D11Device* device, std::wstring filename);
	
	ID3D11ShaderResourceView* GetTextureFromInstant(UINT textureID);
	ID3D11ShaderResourceView* GetTextureFromOpen(UINT textureID);

	void ReleaseTextureFromInstant(UINT textureID);
	void ReleaseTextureFromOpen(UINT textureID);

private:
	std::vector<ID3D11ShaderResourceView*> m_instantCache;
	std::vector<ID3D11ShaderResourceView*> m_openCache;
	std::vector<std::wstring> m_openCacheFileNames;

	UINT m_totalInstantCacheID;
	UINT m_totalOpenCacheID;
};