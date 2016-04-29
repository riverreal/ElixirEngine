cbuffer MatrixBuffer 
{
	matrix gWorld;
	matrix gView;
	matrix gProj;
};

struct VertexIn
{
	float3 PosL: POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VertexOut SkyVS(VertexIn input)
{
	VertexOut output;

	output.PosH = mul(float4(input.PosL, 1.0f), gWorld);
	output.PosH = mul(output.PosH, gView);
	output.PosH = mul(output.PosH, gProj);
	//output.PosH = output.PosH.xyww;

	//matrix worldViewProj = gWorld * gView * gProj;
	//output.PosH = mul(float4(input.PosL, 1.0f), worldViewProj).xyww;
	output.PosL = input.PosL;

	return output;
}