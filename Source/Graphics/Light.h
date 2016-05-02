#pragma once

#include <DirectXMath.h>
#include <vector>
#include "../Helper/LightHelper.h"



class Light
{
public:
	///Creates Default Light Scene
	/// 1 Directional light only
	/// There is no Create Directional Light Function since it's created in constructor.
	Light();
	~Light();

	void CreatePointLight(PBRPointLight pointLight);
	void CreatePointLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range);

	PBRDirectionalLight GetDirectionalLight();
	PBRPointLight GetPointLight(int index);
	PBRSpotLight GetSpotLight(int index);

private:
	const unsigned int MAX_POINT_LIGHTS = 15;
	std::vector<PBRPointLight> m_pointLightVector;
	std::vector<PBRSpotLight> m_spotLightVector;
	PBRDirectionalLight m_directionalLight;
};