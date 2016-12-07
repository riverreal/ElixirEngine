cbuffer depthMatrixBuffer
{
	matrix gWorldMatrix;
	matrix gViewMatrix;
	matrix gProjMatrix;
	matrix gTextTrans;
};

struct VIn
{
	float4 position: POSITION;
	float2 tex: TEXCOORD;
};

struct PSIn
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD;
	float4 depthPos: TEXTURE;
};

PSIn ShadowMapShader(VIn input)
{
	PSIn output;

	input.position.w = 1.0f;

	output.position = mul(input.position, gWorldMatrix);
	output.position = mul(output.position, gViewMatrix);
	output.position = mul(output.position, gProjMatrix);

	output.tex = mul(float4(input.tex, 0.0f, 1.0f), gTextTrans).xy;

	output.depthPos = output.position;

	return output;
}