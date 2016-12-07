#pragma once

#include <DirectXMath.h>
#include <vector>
#include "../Helper/LightHelper.h"
#include "../Helper/TypeHelper.h"

class Light
{
public:
	///Creates Default Light Scene
	/// 1 Directional light only
	/// There is no Create Directional Light Function since it's created in constructor.
	Light();
	~Light();

	//Creates a point light and adds it to the scene
	void CreatePointLight(PBRPointLight pointLight);
	//Creates a point light and adds it to the scene
	//Receives:
	//Position X, Y, Z,
	//Color R, G, B, (0.0f - 1.0f)
	//Range
	void CreatePointLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range);
	//Creates a spot light and adds it to the scene
	void CreateSpotLight(PBRSpotLight spotLight);
	//Creates a spot light and adds it to the scene
	//Receives: 
	//Position X, Y, Z,
	//Color R, G, B, (0.0f - 1.0f)
	//Range,
	//Direction X, Y, Z
	//Spot(angle)
	//Attenuation Const, Linear, Quadrantic
	void CreateSpotLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range, DirectX::XMFLOAT3 dir, float spot, float constAtt, float linearAtt, float quadranticAtt);

	//Get a copy of the directional light
	PBRDirectionalLight GetDirectionalLight();
	//Get the modifyable version of the directional light
	PBRDirectionalLight* GetModDirectionalLight();
	
	void SetDirectionalLight(PBRDirectionalLight dirL);

	PBRPointLight GetPointLight(int index);
	PBRSpotLight GetSpotLight(int index);
	//GetModPointLight:
	//Returns a pointer to the point light in the index specified
	//This enables direct modification of the light
	PBRPointLight* GetModPointLight(int index);
	//GetModSpotLight:
	//Returns a pointer to the spot light in the index specified
	//This enables modification on the light directly
	PBRSpotLight* GetModSpotLight(int index);

	//Returns bounding sphere for shadow map calculation
	BoundingSphere GetSceneBoundary();
	//Returns light's projection matrix for shadow map calculation
	DirectX::XMMATRIX GetLightProjMatrix();
	//Returns light's view matrix for shadow map calculation
	DirectX::XMMATRIX GetLightViewMatrix();
	//Returns light's position for shadow map calculation
	DirectX::XMFLOAT3 GetLightPosition();

	DirectX::XMMATRIX GetLightTransform();

	void BuildShadowTransform();

	U32 GetPointLightCount();
	U32 GetSpotLightCount();

private:
	const U32 MAX_POINT_LIGHTS = 50;
	const U32 MAX_SPOT_LIGHTS = 20;
	std::vector<PBRPointLight> m_pointLightVector;
	std::vector<PBRSpotLight> m_spotLightVector;
	PBRDirectionalLight m_directionalLight;
	U32 m_pointLightCount;
	U32 m_spotLightCount;
	BoundingSphere m_sceneBoundary;

	XMMATRIX m_lightView;
	XMMATRIX m_lightProj;
	XMMATRIX m_lightTransform;
};