cbuffer fixed
{
	float2 gTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};
};

cbuffer MatrixBuffer
{
	matrix gWorld;
	matrix gView;
	matrix gProj;
	float3 gEyePosW;
};

struct VertexOut
{
	float3 CenterW: POSITION;
	float2 SizeW : SIZE;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW: NORMAL;
	float2 Tex : TEXCOORD;
	uint PrimID : SV_PRIMITIVEID;
};

[maxvertexcount(4)]
void GS(point VertexOut gin[1],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	gin[0].CenterW = mul(gin[0].CenterW, gWorld).xyz;
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gin[0].CenterW;
	look.y = 0.0f;
	look = normalize(look);
	float3 right = cross(up, look);

	float halfWidth = 0.5f * gin[0].SizeW.x;
	float halfHeight = 0.5f * gin[0].SizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

	GeoOut gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.PosH = mul(v[i], gView);
		gout.PosH = mul(v[i], gProj);
		gout.PosW = v[i].xyz;
		gout.NormalW = look;
		gout.Tex = gTexC[i];
		gout.PrimID = primID;

		triStream.Append(gout);
	}
}