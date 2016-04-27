Texture2D gAlbedo : register(t0);
//Texture2D gRoughness;
//Texture2D gReflectiveness;
//Texture2D gMetallic;

SamplerState samAnisotropic : register(s0);

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PixelOutputType
{
	float4 albedo : SV_Target0;
	float4 normal : SV_Target1;
};

PixelOutputType DeferredPixelShader(PixelInputType input) : SV_TARGET
{
	PixelOutputType output;
	
	output.albedo = gAlbedo.Sample(samAnisotropic, input.tex);
	output.normal = float4(input.normalW, 1.0f);
	//yet to implement other buffers

	return output;
}