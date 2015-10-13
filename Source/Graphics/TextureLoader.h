#pragma once

#include <d3d11.h>

class TextureLoader
{
public:
	static ID3D11ShaderResourceView* CreateDDSTexture(ID3D11Device* device, LPCWSTR filePath);
	static ID3D11ShaderResourceView* CreateWICTexture(ID3D11Device* device, LPCWSTR filePath);
};