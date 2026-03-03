#include "pch.h"
ECS* ECS::instance = nullptr;

ECS ECS::GetInstance()
{
	if (instance == nullptr) 
	{ 
		instance = new ECS(); 
	} 
	return *instance;
}

int ECS::getEntityCount()
{
	return instance->realEntityCount;
}

void ECS::Release(Entity& entityID)
{
	for (auto& pool : instance->componentPools) {
		pool.second->entityDestroyed(entityID);
	}
	realEntityCount--;
	entityID = -1;

}
