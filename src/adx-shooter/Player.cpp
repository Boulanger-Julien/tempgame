#include "pch.h"
#include "Player.h"
#include "Bullet.h"
#include "GameManager.h"

Player::Player() {
	mp_entity = ECS::GetInstance().createEntity();
	ECS::GetInstance().addComponent<transformComponent>(mp_entity, transformComponent());
	ECS::GetInstance().addComponent<ColliderComponent>(mp_entity, ColliderComponent(mp_entity));
	// TODO playercontroller things
}

void Player::Update() {
	// perhaps health regen here
}

void Player::takeDamage(int amount) {
	m_healthPoints -= amount;
	if (m_healthPoints <= 0) {
		m_healthPoints = 0;
		// Game Over stuff (maybe check hp<=0 in updt instead)
	}
}

void Player::Shoot() {

	GameManager::GetInstance().AddBullet();
}