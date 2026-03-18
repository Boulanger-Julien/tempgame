#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"
#include "adx-engine\framework.h"
#include "Basic_Sword.h"

Player::Player() {
	m_entity = ECS::GetInstance().createEntity(transformComponent(0,2,0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(m_entity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(m_entity);
	equippedWeapon = new Basic_Sword();
	mCollider.depth = mTransform.scale.z;
	mCollider.width = mTransform.scale.x;
	mCollider.height = mTransform.scale.y;
	mCollider.compOwner = m_entity;
	mCollider.updateCollider();
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

void Player::Update(const Ray& mouseRay) {
	float deltatime = Timer::GetInstance()->GetDeltatime();

	// Health Regen (1hp/s)
	if (Stats.mCurrentHealthRegenCooldown >= Stats.mHealthRegenCooldown && Stats.mCurrentHealth < Stats.mMaxHealth) {
		Stats.mCurrentHealth += 1;
		Stats.mCurrentHealthRegenCooldown = 0;
	}
	Stats.mCurrentHealthRegenCooldown += deltatime;

	float t = (mTransform.position.y - XMVectorGetY(mouseRay.origin)) / XMVectorGetY(mouseRay.direction);
	XMVECTOR intersectPoint = XMVectorAdd(mouseRay.origin, XMVectorScale(mouseRay.direction, t));
	float targetX = XMVectorGetX(intersectPoint);
	float targetZ = XMVectorGetZ(intersectPoint);
	float dx = targetX - mTransform.position.x;
	float dz = targetZ - mTransform.position.z;
	float angle = atan2f(dx, dz);
	mTransform.rotation.y = angle;
	ECS::GetInstance().getComponent<transformComponent>(m_entity) = mTransform;
	transformComponent weaponTransform = mTransform;
	transformSystem::RotateAround(weaponTransform, mTransform, 1.5f);
	ECS::GetInstance().getComponent<transformComponent>(equippedWeapon->GetEntity()) = weaponTransform;
}

void Player::takeDamage(int damage) {
	Stats.mCurrentHealth -= (damage - Stats.mDefense) > 0 ? (damage - Stats.mDefense) : 0;
	if (Stats.mCurrentHealth <= 0) {
		Stats.mCurrentHealth = 0;
	}
}