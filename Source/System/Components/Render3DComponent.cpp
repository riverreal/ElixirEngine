#include "Render3DComponent.h"

void Elixir::Renderer3D::Initialize()
{
	Enabled = true;
	EnableBackFaceCulling = true;
	CastShadow = true;
}