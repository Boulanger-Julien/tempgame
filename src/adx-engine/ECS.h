#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Entity.h"

class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entityID) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
private:
	std::vector<T> componentData;

public:
	std::unordered_map<int, size_t> entityToIndex;
	std::unordered_map<size_t, int> indexToEntity;

	void insert(Entity entityID, T component);

	T& getData(Entity entityID);

	void entityDestroyed(Entity entityID) override;
};

class ECS {
private:
	int entityIDCounter;
	int realEntityCount;
	std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentPools;

	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentPool();

	static ECS* instance;
	
	std::vector<Entity> entities;
public:
	ECS()
	{
		entityIDCounter = 100; // Start from to avoid conflicts
		realEntityCount = 0;
		if (instance == nullptr)
		{
			instance = this;
		}

	}

	static ECS GetInstance();

	template<typename ...T>
	int createEntity(T && ...components);

	int getEntityCount();

	template<typename T>
	bool hasComponent(Entity entity);

	template<typename T>
	void addComponent(Entity entity, T&& component);

	template<typename T>
	T& getComponent(Entity entity);

	void Release(Entity& entityID);

	std::vector<Entity>& getEntities() { return instance->entities; }
};

template<typename T>
inline void ComponentArray<T>::insert(Entity entityID, T component) {
	entityToIndex[entityID] = componentData.size();
	indexToEntity[componentData.size()] = entityID;
	componentData.push_back(component);
}

template<typename T>
inline T& ComponentArray<T>::getData(Entity entityID) {
	return componentData[entityToIndex[entityID]];
}

template<typename T>
inline void ComponentArray<T>::entityDestroyed(Entity entityID) {
	if (entityToIndex.find(entityID) != entityToIndex.end()) {
		size_t indexOfRemovedEntity = entityToIndex[entityID];
		size_t indexOfLastElement = componentData.size() - 1;
		componentData[indexOfRemovedEntity] = componentData[indexOfLastElement];
		int entityOfLastElement = indexToEntity[indexOfLastElement];
		entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
		indexToEntity[indexOfRemovedEntity] = entityOfLastElement;
		entityToIndex.erase(entityID);
		indexToEntity.erase(indexOfLastElement);
		componentData.pop_back();
	}
}


#include "ECS.inl"