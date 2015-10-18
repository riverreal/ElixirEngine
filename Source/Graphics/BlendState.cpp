#include "BlendState.h"

ID3D11BlendState* BlendState::BSAlphaCoverage = 0;
ID3D11BlendState* BlendState::BSTransparent = 0;
ID3D11BlendState* BlendState::BSRenderTargetWriteOff = 0;

bool BlendState::Init(ID3D11Device* device)
{
	HRESULT result;

	//Alpha To Coverage
	D3D11_BLEND_DESC alphaCovDesc = { 0 };
	alphaCovDesc.AlphaToCoverageEnable = true;
	alphaCovDesc.IndependentBlendEnable = false;
	alphaCovDesc.RenderTarget[0].BlendEnable = false;
	alphaCovDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = device->CreateBlendState(&alphaCovDesc, &BSAlphaCoverage);
	if (FAILED(result))
	{
		return false;
	}

	//Transparent
	D3D11_BLEND_DESC transDesc = { 0 };
	transDesc.AlphaToCoverageEnable = false;
	transDesc.IndependentBlendEnable = false;

	transDesc.RenderTarget[0].BlendEnable = true;
	transDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = device->CreateBlendState(&transDesc, &BSTransparent);
	if (FAILED(result))
	{
		return false;
	}


	D3D11_BLEND_DESC offDesc = { 0 };
	offDesc.AlphaToCoverageEnable = true;
	offDesc.IndependentBlendEnable = false;
	offDesc.RenderTarget[0].BlendEnable = false;
	

	result = device->CreateBlendState(&offDesc, &BSRenderTargetWriteOff);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void BlendState::Shutdown()
{
	ReleaseCOM(BSTransparent);
	ReleaseCOM(BSAlphaCoverage);
	ReleaseCOM(BSRenderTargetWriteOff);
}