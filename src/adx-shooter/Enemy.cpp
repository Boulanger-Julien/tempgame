#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"

Enemy::Enemy() {
	m_entity = ECS::GetInstance().createEntity();
	ECS::GetInstance().addComponent<transformComponent>(m_entity, transformComponent());
	ECS::GetInstance().addComponent<ColliderComponent>(m_entity, ColliderComponent(m_entity, 2.f, 2.f, 2.f));
	ECS::GetInstance().addComponent<velocityComponent>(m_entity, velocityComponent(1.5f, 1, 1.5f));
}

Enemy::~Enemy() {
	ECS::GetInstance().Release(m_entity);
}

void Enemy::Update() {
	float deltatime = Timer::GetInstance()->GetDeltatime();
	
	if (m_currentShootCooldown >= m_shootCooldown) {
		canShoot = true;
		m_currentShootCooldown = 0;
	}
	if (not canShoot) {
		m_currentShootCooldown += deltatime;
	}
}

void Enemy::LookAt(Entity target)
{
	transformComponent& enemyTrans = ECS::GetInstance().getComponent<transformComponent>(m_entity);
	transformComponent& targetTrans = ECS::GetInstance().getComponent<transformComponent>(target);
	FLOAT3 direction = targetTrans.position - enemyTrans.position;
	float desiredYaw = atan2(direction.x, direction.z);
	enemyTrans.rotation.y = desiredYaw;
	//player is behind him, so look at him with the back
	enemyTrans.rotation.y += 3.1415;
}
