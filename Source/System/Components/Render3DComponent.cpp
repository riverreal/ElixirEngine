#include "Render3DComponent.h"

void Elixir::Renderer3D::Initialize()
{
	Enabled = true;
	EnableBackFaceCulling = true;
	CastShadow = true;

	//Hard coded default materials ID set by Texture Manager
	Material.albedo = 1;
	Material.normal = 2;
	Material.roughness = 3;
	Material.metallic = 4;
}