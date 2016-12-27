#pragma once
#include "../../Interface/IComponent.h"
#include "../../Helper/TypeHelper.h"
#include <DirectXMath.h>

namespace Elixir
{
	class Transform : public IComponent
	{
	public:
		virtual void Initialize() override;

	public:
		Vec3f Position;
		Vec3f Rotation;
		Vec3f Scale;

		DirectX::XMFLOAT4X4 World4x4;

		//Set to false if the object is
		//static (Does not perform any kind of transform)
		bool Dynamic;

		Vec3f TextureRotation;
		Vec3f TexturePosition;
		Vec3f TextureScale;

		DirectX::XMFLOAT4X4 TextureTransform4x4;
	};
}