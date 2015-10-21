struct VertexIn
{
	float3 PosW : POSITION;
	float2 SizeW : SIZE;
};

struct VertexOut
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
};

VertexOut VS(VertexIn input)
{
	VertexOut output;

	output.CenterW = input.PosW;
	output.SizeW = input.SizeW;

	return output;
}