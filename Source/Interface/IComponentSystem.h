#pragma once

#include "../System/GameObject.h"

namespace Elixir
{
	class IComponentSystem
	{
	public:

		virtual void Initialize(GameObject* object) = 0;

		virtual void Update(GameObject* object, F32 deltaTime) = 0;
	};
}