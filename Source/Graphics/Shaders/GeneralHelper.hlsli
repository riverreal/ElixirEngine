
//Calculates the normals providing the normal map and tangent
float3 CalcNormal(float3 normalMap, float3 surfaceNormalW, float3 tangentW)
{
	//convert [0,1] to [-1, 1]
	float3 normalT = 2.0f * normalMap - 1.0f;
	normalT = normalize(normalT);
	//convert [0,1] to [-1, 1]
	float3 N = 2.0f * surfaceNormalW - 1.0f;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	//This is why we don't need provide binormals, we can calculate it.
	float3 B = normalize(cross(N, T));


	if (dot(cross(N, T), B) < 0.0)
		T = T * -1.0;

	float3x3 TBN = float3x3(T, B, N);

	float3 result = mul(normalT, TBN);
	result = normalize(result);
	//convert to [0, 1]
	result = 0.5f * (result + 1.0f);
	result = saturate(result);

	return result;
}