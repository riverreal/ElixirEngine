TextureCube gCubeMap;

SamplerState samAnisotropic;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

float4 SkyPS(VertexOut input) : SV_TARGET
{
	return gCubeMap.Sample(samAnisotropic, input.PosL);
}