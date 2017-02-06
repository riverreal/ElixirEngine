#pragma once

#include "../../Interface/IComponent.h"
#include "../../Helper/TypeHelper.h"
#include <DirectXMath.h>

namespace Elixir
{
	//Renderer does not have a system since it works internally with the engine.
	class Renderer3D : public IComponent
	{
	public:

		virtual void Initialize() override;

	public:
		bool Enabled;
		bool EnableBackFaceCulling;
		bool CastShadow;

		Material Material;

		//Stores the offset of the model in the respective vertex buffer
		offsetData Model;

		std::string ModelPath;
		bool ModelTypePrimitive = false;
		U32 PrimitiveType;
	};

	
}