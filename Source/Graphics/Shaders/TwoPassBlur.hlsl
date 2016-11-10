/**************************************************************
//                      Desolate Engine
//
// Two Pass Blur Pixel Shader
// Uses Gaussian to Blur the input image. It has to be passed twice in order to get
// both axis blured.
// Passes are controlled with a boolean (float actially). First pass(0) is always horizontal.
//
***************************************************************/
Texture2D gImg : register(t0);

SamplerState samplerLinear;

cbuffer passProperties
{
	float passNumber;
	float gWidth;
	float gHeight;
	float pad;
};

struct VertexOut
{
	float4 PosL : SV_POSITION;
	float2 tex : TEXCOORD0;
};

const float offset[] = { -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f };
const float weight[] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
const float BlurWeights[7] =
{
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
};

float SCurve(float x)
{
	x = x*2.0f - 1.0f;
	return -x * abs(x) * 0.5f + x + 0.5f;
}

float4 TwoPassBlur(VertexOut input) : SV_TARGET
{
	float3 output = gImg.Sample(samplerLinear, input.tex).rgb;
	
	float2 coord;
	float radius = 6.0f;

	if (passNumber < 1.0f)
	{
		
		//Horizontal Blur

		if (radius >= 1.0f)
		{
			float4 A = float4(0.0, 0.0, 0.0, 0.0);
			float4 C = float4(0.0, 0.0, 0.0, 0.0);

			float width = 1.0 / gWidth;
			float divisor = 0.0;
			float weight = 0.0;

			float radiusMult = 1.0f / radius;

			for (int i = -radius; i <= radius; i++)
			{
				float2 coord = input.tex + float2(i * width, 0.0);
				coord = saturate(coord);
				A = gImg.Sample(samplerLinear, coord);
				weight = SCurve(1.0f - (abs(i) * radiusMult));

				C += A*weight;

				divisor += weight;
			}

			return float4(C.r / divisor, C.g / divisor, C.b / divisor, 1.0);
		}
		
		/*
		float texelSize = 1.0f / gWidth;

		for (int i = 0; i < 7; i++)
		{
			float2 coord = input.tex + float2((i - 3) * texelSize, 0.0);
			output += gImg.Sample(samplerLinear, coord) * BlurWeights[i];
		}
		*/
	}
	else
	{
		
		if (radius >= 1.0f)
		{
			float4 A = float4(0.0, 0.0, 0.0, 0.0);
			float4 C = float4(0.0, 0.0, 0.0, 0.0);

			float height = 1.0 / gHeight;
			float divisor = 0.0;
			float weight = 0.0;

			float radiusMult = 1.0f / radius;

			for (int i = -radius; i <= radius; i++)
			{
				float2 coord = input.tex + float2(0.0f, i * height);
				coord = saturate(coord);
				A = gImg.Sample(samplerLinear, coord);
				weight = SCurve(1.0f - (abs(i) * radiusMult));

				C += A*weight;

				divisor += weight;
			}

			return float4(C.r / divisor, C.g / divisor, C.b / divisor, 1.0);
		}
		

		/*
		float texelSize = 1.0f / gHeight;

		for (int i = 0; i < 7; i++)
		{
			float2 coord = input.tex + float2(0.0f, (i - 3) * texelSize);
			output += gImg.Sample(samplerLinear, coord) * BlurWeights[i];
		}
		*/
	}

	return float4(output, 1.0f);
}