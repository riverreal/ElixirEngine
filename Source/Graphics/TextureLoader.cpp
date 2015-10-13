#include "TextureLoader.h"
#include "../../External Soruce/DDSTextureLoader/DDSTextureLoader.h"
#include "../../External Soruce/WICTextureLoader/WICTextureLoader.h"

ID3D11ShaderResourceView* TextureLoader::CreateDDSTexture(ID3D11Device* device, LPCWSTR filePath)
{
	ID3D11ShaderResourceView* textureView;
	HRESULT result = DirectX::CreateDDSTextureFromFile(device, filePath, nullptr, &textureView);
	if (FAILED(result))
	{
		MessageBox(0, L"Can't Load!", filePath, MB_OK);
		return textureView;
	}

	return textureView;
}

ID3D11ShaderResourceView* TextureLoader::CreateWICTexture(ID3D11Device* device, LPCWSTR filePath)
{
	ID3D11ShaderResourceView* textureView;
	HRESULT result = DirectX::CreateWICTextureFromFile(device, filePath, nullptr, &textureView);
	if (FAILED(result))
	{
		
		MessageBox(0, L"Can't Load!", filePath, MB_OK);
		return textureView;
	}

	return textureView;
}