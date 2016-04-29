TextureCube gCubeMap : register(t0);

SamplerState samAnisotropic;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

struct PixelOut
{
	float4 albedo : SV_Target0;
};

PixelOut SkyPS(VertexOut input) : SV_TARGET
{
	PixelOut output;
	output.albedo = gCubeMap.Sample(samAnisotropic, input.PosL);
	return output;
}