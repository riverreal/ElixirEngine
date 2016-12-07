cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix texTransf;
};

struct VertexInputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangentW : TANGENT;
};

PixelInputType DeferredVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;

	//position
	output.positionW = mul(float4(input.position, 1.0f), worldMatrix).rgb;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	float4 viewPosition = mul(output.position, viewMatrix);
	output.position = mul(viewPosition, projectionMatrix);

	//output.position = input.position;
	//texture
	output.tex = mul(float4(input.tex, 0.0f, 1.0f), texTransf).xy;
	
	//normal
	output.normalW = mul(input.normal, (float3x3)worldMatrix);
	output.normalW = normalize(output.normalW);

	//convert from [-1, 1] to [0, 1]
	output.normalW.rgb = 0.5f * (output.normalW.rgb + 1.0f);

	output.tangentW = mul(input.tangent, (float3x3)worldMatrix);
	output.tangentW = normalize(output.tangentW);
	//output.tangentW.rgb = 0.5f * (output.tangentW.rgb + 1.0f);

	return output;
}