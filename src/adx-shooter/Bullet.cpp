#include "pch.h"
#include "Bullet.h"
#include "adx-core/Timer.h"

Bullet::Bullet() {
	m_entity = ECS::GetInstance().createEntity(transformComponent(), ColliderComponent());
	ECS::GetInstance().getComponent<ColliderComponent>(m_entity).compOwner = m_entity;
}

Bullet::~Bullet() {
	ECS::GetInstance().Release(m_entity);
}

void Bullet::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();

	if (currentLifetime >= maxLifetime) {
		toBeDestroyed = true;
		return;
	}
	currentLifetime += deltaTime;

	transformSystem::MoveForward(ECS::GetInstance().getComponent<transformComponent>(m_entity), m_speed*deltaTime);
}
