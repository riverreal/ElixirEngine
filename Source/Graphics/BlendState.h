#pragma once

#include "../Helper/GeneralHelper.h"
#include <d3d11.h>

class BlendState
{
public:
	static bool Init(ID3D11Device* device);
	static void Shutdown();

	static ID3D11BlendState* BSAlphaCoverage;
	static ID3D11BlendState* BSTransparent;
	static ID3D11BlendState* BSOff; //Or just set NULL at OMSetBlendState
};