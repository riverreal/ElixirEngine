#include "Light.h"

using namespace DirectX;

Light::Light()
	:m_pointLightCount(0),
	m_spotLightCount(0)
{
	m_directionalLight = PBRDirectionalLight();
	m_directionalLight.LightColor[0] = 0.3f;
	m_directionalLight.LightColor[1] = 0.3f;
	m_directionalLight.LightColor[2] = 0.3f;
	m_directionalLight.LightColor[3] = 1.0f;
	m_directionalLight.Direction[0] = 0.0f;
	m_directionalLight.Direction[1] = -1.0f;
	m_directionalLight.Direction[2] = 0.0f;
	m_directionalLight.LightIntensity[0] = 0.6f;
	m_directionalLight.LightIntensity[1] = 1.0f;
	m_directionalLight.LightIntensity[2] = 1.0f;
	m_directionalLight.LightIntensity[3] = 1.0f;
	m_sceneBoundary.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);
}

Light::~Light()
{
}

void Light::CreatePointLight(PBRPointLight pointLight)
{
	if (m_pointLightCount < MAX_POINT_LIGHTS)
	{
		m_pointLightVector.push_back(pointLight);
		m_pointLightCount++;
	}
}

void Light::CreatePointLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range)
{
	if (m_pointLightCount < MAX_POINT_LIGHTS)
	{
		PBRPointLight pointLight(posX, posY, posZ, lightColorR, lightColorG, lightColorB, range);
		m_pointLightVector.push_back(pointLight);
		m_pointLightCount++;
	}
}

void Light::CreateSpotLight(PBRSpotLight spotLight)
{
	if (m_spotLightCount < MAX_SPOT_LIGHTS)
	{
		m_spotLightVector.push_back(spotLight);
		m_spotLightCount++;
	}
}

void Light::CreateSpotLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range, DirectX::XMFLOAT3 dir, float spot, float constAtt, float linearAtt, float quadranticAtt)
{
	if (m_spotLightCount < MAX_SPOT_LIGHTS)
	{
		PBRSpotLight spotLight(posX, posY, posZ, lightColorR, lightColorG, lightColorB, range, spot, constAtt, linearAtt, quadranticAtt, dir.x, dir.y, dir.z);
		m_spotLightVector.push_back(spotLight);
		m_spotLightCount++;
	}
}

PBRDirectionalLight Light::GetDirectionalLight()
{
	return m_directionalLight;
}

PBRDirectionalLight * Light::GetModDirectionalLight()
{
	return &m_directionalLight;
}

void Light::SetDirectionalLight(PBRDirectionalLight dirL)
{
	m_directionalLight = dirL;
}

PBRPointLight Light::GetPointLight(int index)
{
	return m_pointLightVector[index];
}

PBRSpotLight Light::GetSpotLight(int index)
{
	return m_spotLightVector[index];
}

PBRPointLight * Light::GetModPointLight(int index)
{
	return &m_pointLightVector[index];
}

PBRSpotLight * Light::GetModSpotLight(int index)
{
	return &m_spotLightVector[index];
}

BoundingSphere Light::GetSceneBoundary()
{
	return m_sceneBoundary;
}

DirectX::XMMATRIX Light::GetLightProjMatrix()
{
	/*
	auto r = m_sceneBoundary.Radius;
	XMMATRIX projMatrix = XMMatrixOrthographicOffCenterLH(-r, r, -r, r, -r * 6.0f, r * 6.0f);
	*/
	return m_lightProj;
}

DirectX::XMMATRIX Light::GetLightViewMatrix()
{
	/*
	XMFLOAT3 upF3(0, 1, 0);
	XMVECTOR up = XMLoadFloat3(&upF3);

	XMFLOAT3 centerF3 = m_sceneBoundary.Center;
	XMVECTOR center = XMLoadFloat3(&centerF3);

	XMFLOAT3 eyeF3 = GetLightPosition();
	XMVECTOR eye = XMLoadFloat3(&eyeF3);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, center, up);
	return viewMatrix;
	*/

	return m_lightView;
}

DirectX::XMFLOAT3 Light::GetLightPosition()
{
	return XMFLOAT3(m_sceneBoundary.Center.x - (m_directionalLight.Direction[0] * m_sceneBoundary.Radius * 2.0f),
		m_sceneBoundary.Center.y - (m_directionalLight.Direction[1] * m_sceneBoundary.Radius * 2.0f),
		m_sceneBoundary.Center.z - (m_directionalLight.Direction[2] * m_sceneBoundary.Radius * 2.0f));
}

DirectX::XMMATRIX Light::GetLightTransform()
{
	return m_lightTransform;
}

void Light::BuildShadowTransform()
{
	XMVECTOR lightDir = XMLoadFloat3(&XMFLOAT3(m_directionalLight.Direction[0], m_directionalLight.Direction[1] , m_directionalLight.Direction[2]));
	XMVECTOR lightPos = -2.0f * m_sceneBoundary.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&m_sceneBoundary.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	float l = sphereCenterLS.x - m_sceneBoundary.Radius;
	float b = sphereCenterLS.y - m_sceneBoundary.Radius;
	float n = sphereCenterLS.z - m_sceneBoundary.Radius;
	float r = sphereCenterLS.x + m_sceneBoundary.Radius;
	float t = sphereCenterLS.y + m_sceneBoundary.Radius;
	float f = sphereCenterLS.z + m_sceneBoundary.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	
	XMMATRIX T(
		0.5f, 0.0f, 0.0f ,0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
		);
		
	XMMATRIX S = V * P * T;

	m_lightProj = P;
	m_lightView = V;
	m_lightTransform = S;
}

U32 Light::GetPointLightCount()
{
	return m_pointLightCount;
}

U32 Light::GetSpotLightCount()
{
	return m_spotLightCount;
}


