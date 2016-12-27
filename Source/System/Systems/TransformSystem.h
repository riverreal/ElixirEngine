#pragma once

#include "../../Interface/IComponentSystem.h"
#include "../GameObject.h"

//Dependencies
#include "../Components/TransformComponent.h"

namespace Elixir
{
	class TransformSystem : public IComponentSystem
	{
	public:
		//TransformSystem();

		virtual void Initialize(GameObject* object) override;
		//Transform System dependencies:
		//-Transform
		virtual void Update(GameObject* object, F32 deltaTime) override;

	private:
		//Check if the object has all the component dependancies
		bool CheckDependancies(GameObject* object);
		void UpdateMatrices(Transform* object);

	private:
		bool m_initialRefreshDone = false;

	};
}