#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"
#include "adx-engine\framework.h"
#include "Basic_Sword.h"


Player::Player() {
	mEntity = ECS::GetInstance().createEntity(transformComponent(0,2,0), ColliderComponent(), HealthComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mHealthComponent = ECS::GetInstance().getComponent<HealthComponent>(mEntity);
	mWeapon = new Basic_Sword();
	mCollider.depth = mTransform.scale.z;
	mCollider.width = mTransform.scale.x;
	mCollider.height = mTransform.scale.y;
	mCollider.compOwner = mEntity;
	mCollider.updateCollider();

	mStats.SetStats(100, 0, 0, 0, 10, 0, 35, 0, 0);
	mHealthComponent.mHealth = mStats.mHealth;
}

void Player::ChooseClass(int classID) {
	switch (classID) {
	case 0: // Warrior
		ECS::GetInstance().addComponent(mEntity, StatsComponent(10, 5, 2, 55, 0, 4.0f, 4, 5, 2));
		mStats = ECS::GetInstance().getComponent<StatsComponent>(mEntity);
		break;
	case 1: // Mage
		ECS::GetInstance().addComponent(mEntity, StatsComponent(5, 2, 1, 35, 0, 5.0f, 20, 4, 5));
		mStats = ECS::GetInstance().getComponent<StatsComponent>(mEntity);
		break;
	case 2: // Rogue
		break;
	default:
		break;
	}
}

void Player::Update(const Ray& mouseRay) {
	float deltatime = Timer::GetInstance()->GetDeltatime();

	OnUpdate(deltatime);



	float t = (mTransform.position.y - XMVectorGetY(mouseRay.origin)) / XMVectorGetY(mouseRay.direction);
	XMVECTOR intersectPoint = XMVectorAdd(mouseRay.origin, XMVectorScale(mouseRay.direction, t));
	float targetX = XMVectorGetX(intersectPoint);
	float targetZ = XMVectorGetZ(intersectPoint);
	float dx = targetX - mTransform.position.x;
	float dz = targetZ - mTransform.position.z;
	float angle = atan2f(dx, dz);
	mTransform.rotation.y = angle;
}
void Player::Update() {
	float deltatime = Timer::GetInstance()->GetDeltatime();

	OnUpdate(deltatime);
}
void Player::OnUpdate(float _deltatime)
{
	HealthSystem::RecoverHealth(mHealthComponent, mStats.mHealthRegen);

	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	transformComponent weaponTransform = mTransform;
	transformSystem::RotateAround(weaponTransform, mTransform, 1.5f);
	ECS::GetInstance().getComponent<transformComponent>(mWeapon->GetEntity()) = weaponTransform;
	transformSystem::MoveByKey(mTransform, mStats.mSpeed, -45, _deltatime);
}
void Player::takeDamage(int _damage) {
	HealthSystem::TakeDamage(mHealthComponent, _damage);
}