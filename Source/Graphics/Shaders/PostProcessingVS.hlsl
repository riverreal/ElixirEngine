struct VertexIn
{
	float3 PosL: POSITION;
};

struct VertexOut
{
	float4 PosL : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VertexOut PostProcessingVS(uint vI: SV_VERTEXID)
{
	VertexOut output;

	//output.PosL = float4(input.PosL, 1.0f);

	output.PosL.x = (float)(vI / 2) * 4.0 - 1.0;
	output.PosL.y = (float)(vI % 2) * 4.0 - 1.0;
	output.PosL.z = 0.0;
	output.PosL.w = 1.0;

	output.tex.x = (float)(vI / 2) * 2.0;
	output.tex.y = 1.0 - (float)(vI % 2) * 2.0;

	return output;
}