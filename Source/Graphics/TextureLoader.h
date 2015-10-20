#pragma once

#include <d3d11.h>
#include <vector>
#include <string>

class TextureLoader
{
public:
	static ID3D11ShaderResourceView* CreateDDSTexture(ID3D11Device* device, LPCWSTR filePath);
	static ID3D11ShaderResourceView* CreateWICTexture(ID3D11Device* device, LPCWSTR filePath);
	static ID3D11ShaderResourceView* CreateDDSTexture2DArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<std::wstring>& filepaths);
};