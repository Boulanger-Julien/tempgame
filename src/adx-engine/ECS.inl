#include "ECS.h"

template<typename T>
inline std::shared_ptr<ComponentArray<T>> ECS::getComponentPool()
{
	auto type = std::type_index(typeid(T));
	if (instance->componentPools.find(type) == instance->componentPools.end()) {
		instance->componentPools[type] = std::make_shared<ComponentArray<T>>();
	}
	return std::static_pointer_cast<ComponentArray<T>>(instance->componentPools[type]);
}

template<typename ...T>
inline int ECS::createEntity(T&&... components)
{
	(void)std::initializer_list<int>{ (instance->addComponent(instance->entityIDCounter, std::forward<T>(components)), 0)... };
	instance->realEntityCount++;
	instance->entities.push_back(instance->entityIDCounter);
	return instance->entityIDCounter++;
}

template<typename T>
inline bool ECS::hasComponent(Entity entity)
{
	auto type = std::type_index(typeid(T));
	if (instance->componentPools.find(type) == instance->componentPools.end()) {
		return false;
	}
	auto pool = std::static_pointer_cast<ComponentArray<T>>(instance->componentPools[type]);
	return pool->entityToIndex.find(entity) != pool->entityToIndex.end();
}

template<typename T>
inline void ECS::addComponent(Entity entity, T&& component)
{
	instance->getComponentPool<T>()->insert(entity, component);
}

template<typename T>
T& ECS::getComponent(Entity entityID) {
	//assert(hasComponent<T>(entityID) && "Tentative de r�cup�rer un composant inexistant !");
	return instance->getComponentPool<T>()->getData(entityID);
}