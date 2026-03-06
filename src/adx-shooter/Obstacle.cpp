#include "pch.h"
#include "Obstacle.h"

Obstacle::Obstacle() {
	m_entity = ECS::GetInstance().createEntity();
	ECS::GetInstance().addComponent<transformComponent>(m_entity, transformComponent());
	ECS::GetInstance().addComponent<ColliderComponent>(m_entity, ColliderComponent(m_entity, 6.f, 2.f, 1.f));
	
}

Obstacle::~Obstacle() {
	ECS::GetInstance().Release(m_entity);
}