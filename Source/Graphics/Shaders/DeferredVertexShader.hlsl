cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD0;
};

PixelInputType DeferredVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;

	//position
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	float4 viewPosition = mul(output.position, viewMatrix);
	output.position = mul(viewPosition, projectionMatrix);

	//output.position = input.position;
	//texture
	output.tex = input.tex;
	
	//normal
	output.normalW = mul(input.normal, (float3x3)worldMatrix);
	output.normalW = normalize(output.normalW);

	return output;
}