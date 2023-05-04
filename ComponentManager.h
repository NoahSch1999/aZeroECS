#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "SystemManager.h"

namespace aZeroECS
{
	struct ComponentArrayBase
	{
		ComponentArrayBase() = default;
		virtual ~ComponentArrayBase() = default;

		/** Pure virtual which is overriden by ComponentArray to allow MappedVector::Remove() without having to specify template parameters.
		@param id Unique Entity ID
		@return void
		*/
		virtual void Remove(int id) = 0;
	};

	template<typename T>
	struct ComponentArray : public ComponentArrayBase
	{
		ComponentArray() = default;

		MappedVector<T> m_components;

		/** Removes the component matching the specified Entity ID.
		@param id Unique Entity ID
		@return void
		*/
		virtual void Remove(int id) override
		{
			m_components.Remove(id);
		}
	};

	/** @brief Used to store, manage, and register components.
	*/
	class ComponentManager
	{
		friend class EntityManager;

	private:
		std::unordered_map<std::type_index, std::unique_ptr<ComponentArrayBase>> m_componentArrayMap;
		std::unordered_map<std::type_index, short> m_typeToBitflag;
		SystemManager& m_systemManager;

	public:
		ComponentManager(SystemManager& systemManager)
			:m_systemManager(systemManager) { }

		/** Registers a new component type of type T for the ComponentManager.
		* This should be called once for each component that the ComponentManager should support.
		@return void
		*/
		template<typename T>
		void RegisterComponent()
		{
			std::type_index typeIndex = std::type_index(typeid(T));

			m_typeToBitflag.emplace(typeIndex, m_componentArrayMap.size());
			m_componentArrayMap.emplace(typeIndex, std::make_unique<ComponentArray<T>>());
		}

		/** Checks whether or not the input Entity currently has a component of type T.
		@param entity The Entity to check
		@return bool TRUE: The Entity has a component of type T, FALSE: The Entity doesn't have a component of type T
		*/
		template<typename T>
		bool HasComponent(const Entity& entity) const
		{
			return entity.m_componentMask.test(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))));
		}

		/** Adds a component of type T to the input Entity.
		@param entity The Entity to add the component to
		@return void
		*/
		template<typename T>
		void AddComponent(Entity& entity)
		{
			entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), true);

			ComponentArray<T>* componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());

			componentArray->m_components.Add(entity.m_id, std::move(T()));

			m_systemManager.EntityUpdated(entity);

		}

		/** Adds a component of type T to the input Entity.
		* Initiates the new component with the input data.
		@param entity The Entity to add the component to
		@param data Initial data for the component
		@return void
		*/
		template<typename T>
		void AddComponent(Entity& entity, const T& data)
		{
			entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), true);

			ComponentArray<T>* componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());

			componentArray->m_components.Add(entity.m_id, data);

			m_systemManager.EntityUpdated(entity);

		}

		/** Returns a pointer to the component of type T for the input Entity.
		* Consider using ComponentManager::GetComponentArray<T>() in conjunctions with ComponentManager::GetComponent(ComponentArray<T>& componentArray, const Entity& entity) when accessing multiple components of the same type in a row.
		* This will avoid the additional ComponentArray<T> lookup time which this method has.
		* This method returns nullptr if the input Entity doesn't have a component of type T.
		* TODO: TRY TO AVOID GOING THROUGH THE MAP TO ACCESS THE COMPONENT ARRAY
		@param entity The Entity to get the component for
		@return void
		*/
		template<typename T>
		T* GetComponent(const Entity& entity)
		{
			ComponentArray<T>* componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());
			if(componentArray->m_components.Contains(entity.m_id))
				return componentArray->m_components.GetObjectByID(entity.m_id);

			return nullptr;
		}

		/** Returns a pointer to the component of type T within the input ComponentArray<T> for the input Entity.
		* This method returns nullptr if the input Entity doesn't have a component of type T.
		* TODO: TRY TO AVOID GOING THROUGH THE MAP TO ACCESS THE COMPONENT ARRAY
		@param componentArray The ComponentArray<T> to get the component from
		@param entity The Entity to get the component for
		@return void
		*/
		template<typename T>
		T* GetComponent(ComponentArray<T>& componentArray, const Entity& entity)
		{
			if (componentArray.m_components.Contains(entity.m_id))
				return componentArray.m_components.GetObjectByID(entity.m_id);

			return nullptr;
		}

		/** Returns a pointer to the component of type T for the input Entity.
		* Consider using ComponentManager::GetComponentArray<T>() in conjunctions with ComponentManager::GetComponent(ComponentArray<T>& componentArray, const Entity& entity) when accessing multiple components of the same type in a row.
		* This will avoid the additional ComponentArray<T> lookup time which this method has.
		* This method will might crash if the Entity doesn't have a component of type T. This avoid unneccessary if-statements. 
		* Consider using ComponentManager::GetComponent() or atleast ComponentManager::HasComponent<T>() if you want to avoid this.
		* TODO: TRY TO AVOID GOING THROUGH THE MAP TO ACCESS THE COMPONENT ARRAY
		@param entity The Entity to get the component for
		@return void
		*/
		template<typename T>
		T* GetComponentFast(const Entity& entity)
		{
			ComponentArray<T>* componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());
			return componentArray->m_components.GetObjectByID(entity.m_id);
		}

		/** Returns a pointer to the component of type T within the input ComponentArray<T> for the input Entity.
		* This method will might crash if the Entity doesn't have a component of type T. This avoid unneccessary if-statements. 
		* Consider using ComponentManager::GetComponent() or atleast ComponentManager::HasComponent<T>() if you want to avoid this.
		* TODO: TRY TO AVOID GOING THROUGH THE MAP TO ACCESS THE COMPONENT ARRAY
		@param componentArray The ComponentArray<T> to get the component from
		@param entity The Entity to get the component for
		@return void
		*/
		template<typename T>
		T* GetComponentFast(ComponentArray<T>& componentArray, const Entity& entity)
		{
			return componentArray.m_components.GetObjectByID(entity.m_id);
		}

		/** Removes the component of type T for the input Entity.
		@param entity The Entity to remove the component for
		@return void
		*/
		template<typename T>
		void RemoveComponent(Entity& entity)
		{
			entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), false);

			ComponentArrayBase* base = m_componentArrayMap.at(std::type_index(typeid(T))).get();
			base->Remove(entity.m_id);

			m_systemManager.EntityUpdated(entity);
		}

		/** Removes the component of type T matching the input std::type_index for the input Entity.
		@param entity The Entity to remove the component for
		@param typeIndex The std::type_index which matches a registered type T component
		@return void
		*/
		void RemoveComponent(Entity& entity, std::type_index typeIndex)
		{
			entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(typeIndex)), false);

			ComponentArrayBase* base = m_componentArrayMap.at(typeIndex).get();
			base->Remove(entity.m_id);

			m_systemManager.EntityUpdated(entity);
		}

		/** Returns a reference to a ComponentArray<T>.
		@return ComponentArray<T>&
		*/
		template<typename T>
		ComponentArray<T>& GetComponentArray()
		{
			return *static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());
		}

		/** Returns the std::bitset bit index for the component of type T.
		@return short
		*/
		template<typename T>
		short GetComponentBit() const
		{
			return m_typeToBitflag.at(std::type_index(typeid(T)));
		}
	};
}