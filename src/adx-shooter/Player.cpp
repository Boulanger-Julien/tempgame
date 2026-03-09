#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"

Player::Player() {
	m_entity = ECS::GetInstance().createEntity(transformComponent(0,2,0), ColliderComponent());
	ECS::GetInstance().getComponent<ColliderComponent>(m_entity).updateCollider();
}

void Player::ChooseClass(int classID) {
	switch (classID) {
	case 0: // Warrior
		ECS::GetInstance().addComponent(m_entity, StatsComponent(10, 5, 2, 55, 0, 4.0f, 4, 5, 2));
		Stats = ECS::GetInstance().getComponent<StatsComponent>(m_entity);
		break;
	case 1: // Mage
		ECS::GetInstance().addComponent(m_entity, StatsComponent(5, 2, 1, 35, 0, 5.0f, 20, 4, 5));
		Stats = ECS::GetInstance().getComponent<StatsComponent>(m_entity);
		break;
	case 2: // Rogue
		break;
	default:
		break;
	}
}

void Player::Update() {
	float deltatime = Timer::GetInstance()->GetDeltatime();

	// Health Regen (1hp/s)
	if (Stats.mCurrentHealthRegenCooldown >= Stats.mHealthRegenCooldown && Stats.mHealthPoints < Stats.mMaxHealthPoints) {
		Stats.mHealthPoints += 1;
		Stats.mCurrentHealthRegenCooldown = 0;
	}
	Stats.mCurrentHealthRegenCooldown += deltatime;
}

void Player::takeDamage(int damage) {
	Stats.mHealthPoints -= (damage - Stats.mDefense) > 0 ? (damage - Stats.mDefense) : 0;
	if (Stats.mHealthPoints <= 0) {
		Stats.mHealthPoints = 0;
	}
}