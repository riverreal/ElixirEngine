#include "TransformComponent.h"

using namespace DirectX;

void Elixir::Transform::Initialize()
{
	Position.x = 0.0f;
	Position.y = 0.0f;
	Position.z = 0.0f;

	Rotation.x = 0.0f;
	Rotation.y = 0.0f;
	Rotation.z = 0.0f;

	Scale.x = 1.0f;
	Scale.y = 1.0f;
	Scale.z = 1.0f;

	Dynamic = true;

	TextureRotation = Vec3f(0.0f);
	TexturePosition = Vec3f(0.0f);
	TextureScale = Vec3f(1.0f);

	XMStoreFloat4x4(&World4x4, XMMatrixIdentity());
	
	XMStoreFloat4x4(&TextureTransform4x4, XMMatrixIdentity());
}