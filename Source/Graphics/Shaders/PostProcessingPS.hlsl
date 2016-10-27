#include "PostProcessing.hlsli"
Texture2D gImg : register(t0);

SamplerState samplerLinear;
//true = 1
//false = 0
#define TONEMAPPING 1
#define FXAA 1

struct VertexOut
{
	float4 PosL : SV_POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelOut
{
	float4 color : SV_Target0;
};

PixelOut PostProcessingPS(VertexOut input) : SV_TARGET
{
	PixelOut output;

#if FXAA
	uint dx, dy;
	gImg.GetDimensions(dx, dy);
	float2 rcpro = rcp(float2(dx, dy));

	FxaaTex t;
	t.smpl = samplerLinear;
	t.tex = gImg;

	//FXAA function samples the texture
	output.color = FxaaPixelShader(input.tex, 0, t, t, t, rcpro, 0, 0, 0, 1.0, 0.166, 0.0312, 0, 0, 0, 0);
#else
	output.color = gImg.Sample(samplerLinear, input.tex);
#endif

#if TONEMAPPING
	output.color *= 2;
	output.color = FilmicTonemap(output.color);
#endif

	return output;
}