#pragma once
#include "TextureLoader.h"
#include "../Helper/TypeHelper.h"


class TextureManager
{
public:
	TextureManager(ID3D11Device* device);
	//Removes all textures by default
	~TextureManager();

	//Adds a texture which it's filepath becomes it's name.
	//Returns it's id
	//ID 0 is reserved
	U32 AddTexture(std::wstring filepath);

	//Add texture for Elixir Editor.
	//Acts like a regular AddTexture but adds +1 to firstID and puts the new item
	//before firstID
	U32 AddEditorTexture(std::wstring filepath);

	//Adds a texture specifying it's name
	//Returns it's id
	//ID 0 is reserved
	U32 AddTexture(std::wstring filepath, std::wstring name);

	void AddAllocatedTexture(std::wstring filepath, U32 ID);

	//Returns a texture by it's name
	ID3D11ShaderResourceView* GetTexture(std::wstring name);

	//Returns a texture by it's id
	ID3D11ShaderResourceView* GetTexture(U32 id);

	std::string GetTextureName(U32 id);

	void AllocateTexture();

	//Get ID by name
	U32 GetID(std::wstring name); //If needed for some reason

	//Remove's a texture by it's name
	void RemoveTexture(U32 id);

	void Shutdown();
	
	ID3D11Device* GetDevice();

	U32 GetMaxID();
	U32 GetFirstID();

private:
	struct TextureItem
	{
		ID3D11ShaderResourceView* srv;
		std::wstring name;
	};

private:
	ID3D11Device* m_device;

	//The index used as id
	U32 m_idCounter;
	U32 m_editorIDCounter;
	U32 m_firstID;
	std::vector<TextureItem* > m_textureVector;

};