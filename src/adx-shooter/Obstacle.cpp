#include "pch.h"
#include "Obstacle.h"

Obstacle::Obstacle() {
	mEntity = ECS::GetInstance().createEntity();
	ECS::GetInstance().addComponent<transformComponent>(mEntity, transformComponent());
	ECS::GetInstance().addComponent<ColliderComponent>(mEntity, ColliderComponent(mEntity, 6.f, 2.f, 1.f));
	
}

Obstacle::~Obstacle() {
	ECS::GetInstance().Release(mEntity);
}