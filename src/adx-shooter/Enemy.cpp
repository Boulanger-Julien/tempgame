#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"

Enemy::Enemy(int _playerIndex) {
	m_entity = ECS::GetInstance().createEntity(transformComponent(0, 2, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(m_entity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(m_entity);
	mCollider.depth = mTransform.scale.z * 2;
	mCollider.width = mTransform.scale.x * 2;
	mCollider.height = mTransform.scale.y * 2;

	mCollider.compOwner = m_entity;
	mCollider.updateCollider();
	mPlayerIndex = _playerIndex;
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
	ECS::GetInstance().getComponent<transformComponent>(m_entity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(m_entity) = mCollider;
	LookAt(mPlayerIndex);

}

void Enemy::LookAt(Entity target)
{
	transformComponent& enemyTrans = ECS::GetInstance().getComponent<transformComponent>(m_entity);
	transformComponent& targetTrans = ECS::GetInstance().getComponent<transformComponent>(target);
	FLOAT3 direction = targetTrans.position - enemyTrans.position;
	float desiredYaw = atan2(direction.x, direction.z);
	enemyTrans.rotation.y = desiredYaw;
	//player is behind him, so look at him with the back
	//enemyTrans.rotation.y += 3.1415;
}
