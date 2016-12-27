#include "TransformSystem.h"
#include <DirectXMath.h>

using namespace DirectX;

void Elixir::TransformSystem::Initialize(GameObject* object)
{
	if (CheckDependancies(object))
	{
		auto transform = object->GetTransform();

		//Don't check whether it's dynamic or not, just update
		UpdateMatrices(transform);
	}
}

void Elixir::TransformSystem::Update(GameObject * object, F32 deltaTime)
{
	if (CheckDependancies(object))
	{
		auto transform = object->GetTransform();

		//Check if it's dynamic
		if (transform->Dynamic)
		{
			UpdateMatrices(transform);
		}
	}
}

bool Elixir::TransformSystem::CheckDependancies(GameObject * object)
{
	bool check = true;
	
	if (object->GetTransform() == nullptr)
	{
		check = false;
	}

	return check;
}

void Elixir::TransformSystem::UpdateMatrices(Transform* transform)
{
	//object world Update
	XMMATRIX scaling = XMMatrixScaling(transform->Scale.x, transform->Scale.y, transform->Scale.z);
	XMMATRIX rotation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(transform->Rotation.y)),
		XMMatrixRotationX(XMConvertToRadians(transform->Rotation.x))), XMMatrixRotationZ(XMConvertToRadians(transform->Rotation.z)));
	XMMATRIX translation = XMMatrixTranslation(transform->Position.x, transform->Position.y, transform->Position.z);
	auto WorldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), translation);

	XMStoreFloat4x4(&transform->World4x4, WorldMatrix);

	//tex transforma Update
	scaling = XMMatrixScaling(transform->TextureScale.x, transform->TextureScale.y, transform->TextureScale.z);
	rotation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(transform->TextureRotation.y)),
		XMMatrixRotationX(XMConvertToRadians(transform->TextureRotation.x))), XMMatrixRotationZ(XMConvertToRadians(transform->TextureRotation.z)));
	translation = XMMatrixTranslation(transform->TexturePosition.x, transform->TexturePosition.y, transform->TexturePosition.z);
	auto TexMatrix = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), translation);

	XMStoreFloat4x4(&transform->TextureTransform4x4, TexMatrix);
}
