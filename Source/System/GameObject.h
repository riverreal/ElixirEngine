#pragma once

#include <unordered_map>
#include <typeinfo>
#include "Components/ComponentIncludes.h"
#include "../Interface/IComponent.h"
#include "../Helper/TypeHelper.h"
#include "../Helper/GeneralHelper.h"

namespace Elixir
{
	//ECS approach
	class GameObject
	{
	public:
		GameObject();
		~GameObject();
		void AddComponent(IComponent* component);
		//Replaces the component if there is already one with the same type.
		template <typename T>
		T* AddComponent();
		template <typename T>
		T* GetComponent();

		template <typename T>
		void RemoveComponent();

		void SetName(std::string name);
		std::string GetName();

		void SetTag(I32 tag);
		I32 GetTag();

		//Return the Transform Component
		Transform* GetTransform();
		//Return the Renderer Component
		Renderer3D* GetRenderer();

		//Returns the vector storing all children
		std::vector<GameObject*> GetChildren();
		void AddChild(GameObject* child);

	private:

	private:

		//Built in pointers to
		//Easy access components
		//Limited to Renderer and Transform

		//Transform Component
		Transform* m_transform;
		//Renderer Component
		Renderer3D* m_renderer;

		//Stores all of the components the GameObject has.
		//Each component type is unique.
		//Meaning that only one of each component is allowed at a time.
		std::unordered_map<const std::type_info*, IComponent*> m_components;

		U32 m_ID;
		std::string m_name;
		I32 m_tag;

		std::vector<GameObject*> m_children;
	};


	template <typename T>
	T* GameObject::AddComponent()
	{
		IComponent* comp = new T;
		AddComponent(comp);
		return static_cast<T*>(comp);
	}

	template <typename T>
	T* GameObject::GetComponent()
	{
		if (m_components.count(&typeid(T)) != 0)
		{
			return static_cast<T*>(m_components[&typeid(T)]);
		}
		else
		{
			//ElixirLog("Could not get component");
			return nullptr;
		}
	}

	template <typename T>
	void GameObject::RemoveComponent()
	{
		if (m_components.count(&typeid(T)) != 0)
		{
			auto comp = GetComponent<T>();

			if (typeid(T) == typeid(Transform))
			{
				m_transform = nullptr;
			}
			else if (typeid(T) == typeid(Renderer3D))
			{
				m_renderer = nullptr;
			}

			SafeRelease(comp);
			m_components.erase(&typeid(T));
		}
		else
		{
			ElixirLog("Could not find component to remove");
		}
	}
}

