struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position: SV_POSITION;	
	float2 tex : TEXCOORD0;	
};

PixelInputType DeferredLightVS(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;

	input.position.w = 1.0f;
	output.position = input.position;
	output.tex = input.tex;

	return output;
}