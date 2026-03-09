#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"

Player::Player() {
	m_entity = ECS::GetInstance().createEntity(transformComponent(0,2,0), velocityComponent(35), ColliderComponent());
	ECS::GetInstance().getComponent<ColliderComponent>(m_entity).updateCollider();
}

void Player::Update() {
	float deltatime = Timer::GetInstance()->GetDeltatime();

	// Health Regen (1hp/s)
	if (mCurrentHealthRegenCooldown >= mHealthRegenCooldown && mHealthPoints < 100) {
		mHealthPoints += 1;
		mCurrentHealthRegenCooldown = 0;
	}
	mCurrentHealthRegenCooldown += deltatime;
}

void Player::takeDamage(int damage) {
	mHealthPoints -= damage;
	if (mHealthPoints <= 0) {
		mHealthPoints = 0;
	}
}