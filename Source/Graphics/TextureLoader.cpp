#include "TextureLoader.h"
#include "../../External Soruce/DDSTextureLoader/DDSTextureLoader.h"
#include "../../External Soruce/WICTextureLoader/WICTextureLoader.h"

ID3D11ShaderResourceView * TextureLoader::CreateTexture(ID3D11Device * device, LPCWSTR filePath)
{
	std::string path;
	path = wstrtostr(filePath);
	auto pathLen = path.length();
	auto fileType = path.substr(pathLen - 4, 4);

	ID3D11ShaderResourceView* result;

	if (fileType == ".dds")
	{
		result = TextureLoader::CreateDDSTexture(device, filePath);
	}
	else
	{
		result = TextureLoader::CreateWICTexture(device, filePath);
	}

	return result;
}

ID3D11ShaderResourceView* TextureLoader::CreateDDSTexture(ID3D11Device* device, LPCWSTR filePath)
{
	ID3D11ShaderResourceView* textureView;
	HRESULT result = DirectX::CreateDDSTextureFromFile(device, filePath, nullptr, &textureView);
	if (FAILED(result))
	{
		MessageBox(0, L"Can't Load!", filePath, MB_OK);
		textureView = nullptr;
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
		textureView = nullptr;
		return textureView;
	}

	return textureView;
}

ID3D11ShaderResourceView* TextureLoader::CreateDDSTexture2DArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<std::wstring>& filepaths)
{
	ID3D11ShaderResourceView* outputTexSRV = 0;
	UINT size = filepaths.size();
	HRESULT result;

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		result = DirectX::CreateDDSTextureFromFile(device, filepaths[i].c_str(), (ID3D11Resource**)&srcTex[i], NULL);
		if (FAILED(result))
		{
			outputTexSRV = nullptr;
			return outputTexSRV;
		}
	}

	D3D11_TEXTURE2D_DESC texDesc;
	srcTex[0]->GetDesc(&texDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDesc.Width;
	texArrayDesc.Height = texDesc.Height;
	texArrayDesc.MipLevels = texDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	result = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	if (FAILED(result))
	{
		outputTexSRV = nullptr;
		return outputTexSRV;
	}

	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			result = deviceContext->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			if (FAILED(result))
			{
				outputTexSRV = nullptr;
				return outputTexSRV;
			}

			deviceContext->Unmap(srcTex[texElement], mipLevel);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	result = device->CreateShaderResourceView(texArray, &viewDesc, &outputTexSRV);
	if (FAILED(result))
	{
		outputTexSRV = nullptr;
		return outputTexSRV;
	}

	delete[] texArray;
	texArray = 0;

	for (UINT i = 0; i < size; ++i)
	{
		delete srcTex[i];
		srcTex[i] = 0;
	}

	return outputTexSRV;
}

ID3D11ShaderResourceView* TextureLoader::CreateDDSCubeMap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<std::wstring>& filepaths)
{
	ID3D11ShaderResourceView* outputTexSRV = 0;
	UINT size = filepaths.size();
	HRESULT result;

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		result = DirectX::CreateDDSTextureFromFile(device, filepaths[i].c_str(), (ID3D11Resource**)&srcTex[i], NULL);
		if (FAILED(result))
		{
			outputTexSRV = nullptr;
			return outputTexSRV;
		}
	}

	D3D11_TEXTURE2D_DESC texDesc;
	srcTex[0]->GetDesc(&texDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDesc.Width;
	texArrayDesc.Height = texDesc.Height;
	texArrayDesc.MipLevels = texDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	result = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	if (FAILED(result))
	{
		outputTexSRV = nullptr;
		return outputTexSRV;
	}

	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			result = deviceContext->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			if (FAILED(result))
			{
				outputTexSRV = nullptr;
				return outputTexSRV;
			}

			deviceContext->Unmap(srcTex[texElement], mipLevel);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	result = device->CreateShaderResourceView(texArray, &viewDesc, &outputTexSRV);
	if (FAILED(result))
	{
		outputTexSRV = nullptr;
		return outputTexSRV;
	}

	delete[] texArray;
	texArray = 0;

	for (UINT i = 0; i < size; ++i)
	{
		delete srcTex[i];
		srcTex[i] = 0;
	}

	return outputTexSRV;
}