#pragma once
#include "TextureLoader.h"
#include "../Helper/TypeHelper.h"


class TextureManager
{
public:
	TextureManager();
	//Removes all textures by default
	~TextureManager();

	//Adds a texture which it's filepath becomes it's name.
	//Returns it's id
	//ID 0 is reserved
	U32 AddTexture(ID3D11Device* device, std::wstring filepath);

	//Adds a texture specifying it's name
	//Returns it's id
	//ID 0 is reserved
	U32 AddTexture(ID3D11Device* device, std::wstring filepath, std::wstring name);

	//Returns a texture by it's name
	ID3D11ShaderResourceView* GetTexture(std::wstring name);

	//Returns a texture by it's id
	ID3D11ShaderResourceView* GetTexture(U32 id);

	//Get ID by name
	U32 GetID(std::wstring name); //If needed for some reason

	//Remove's a texture by it's name
	void RemoveTexture(U32 id);

	void Shutdown();
	

private:
	struct TextureItem
	{
		ID3D11ShaderResourceView* srv;
		std::wstring name;
	};

private:
	//The index used as id
	U32 m_idCounter;
	std::vector<TextureItem* > m_textureVector;

};