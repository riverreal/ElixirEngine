#pragma once

#include "../../Interface/IComponentSystem.h"
#include "../GameObject.h"



namespace Elixir
{
	//Inherit this to create script
	class ScriptSystem : public IComponentSystem
	{
	public:
		virtual void Initialize(GameObject* object) override;

		virtual void Update(GameObject* object, F32 deltaTime) override;

	};
}