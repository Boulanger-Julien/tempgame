#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"

Player::Player() {
	m_entity = ECS::GetInstance().createEntity();
	ECS::GetInstance().addComponent<transformComponent>(m_entity, transformComponent());
	ECS::GetInstance().addComponent<ColliderComponent>(m_entity, ColliderComponent(m_entity));

	ECS::GetInstance().getComponent<transformComponent>(m_entity).position = FLOAT3(0, 3, 0);
	baseY = ECS::GetInstance().getComponent<transformComponent>(m_entity).position.y;
}

void Player::Update() {
	float deltatime = Timer::GetInstance()->GetDeltatime() * m_timeSpeed;

	// Crouch
	if (isCrouching) {
		ECS::GetInstance().getComponent<transformComponent>(m_entity).position.y = baseY - 1;
	}
	else {
		if (ECS::GetInstance().getComponent<transformComponent>(m_entity).position.y != baseY) {
			ECS::GetInstance().getComponent<transformComponent>(m_entity).position.y = baseY;
		}
	}

	// Health Regen (1hp/s)
	if (m_currentHealthRegenCooldown >= m_healthRegenCooldown && m_healthPoints < 100) {
		m_healthPoints += 1;
		m_currentHealthRegenCooldown = 0;
	}
	m_currentHealthRegenCooldown += deltatime;

	// Shoot
	if (m_currentShootCooldown >= m_shootCooldown) {
		canShoot = true;
		m_currentShootCooldown = 0;
	}
	if (not canShoot) {
		m_currentShootCooldown += deltatime;
	}
}

void Player::takeDamage() {
	m_healthPoints -= 15;
	if (m_healthPoints <= 0) {
		m_healthPoints = 0;
	}
}

void Player::SlowDown(bool slow) {
	m_timeSpeed = slow ? 0.5f : 1.0f;
}
