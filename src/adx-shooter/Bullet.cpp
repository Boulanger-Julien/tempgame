#include "pch.h"
#include "Bullet.h"
#include "GameManager.h"

Bullet::Bullet() {
	m_entity = ECS::GetInstance().createEntity(transformComponent(), ColliderComponent(m_entity));
}

void Bullet::Update() {
	float deltaTime = GameManager::GetInstance().GetDeltatime();

	if (currentLifetime >= maxLifetime) {
		toBeDestroyed = true;
		return;
	}
	currentLifetime += deltaTime;

	transformSystem::Move(ECS::GetInstance().getComponent<transformComponent>(m_entity), 0, 0, m_speed*deltaTime);
}

Bullet::~Bullet() {
	ECS::GetInstance().Release(m_entity); // check if components free too
}