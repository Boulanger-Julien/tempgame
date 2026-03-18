#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"

Enemy::Enemy() {
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 2, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mCollider.depth = mTransform.scale.z * 2;
	mCollider.width = mTransform.scale.x * 2;
	mCollider.height = mTransform.scale.y * 2;

	mCollider.compOwner = mEntity;
	mCollider.updateCollider();
}

Enemy::~Enemy() {
	ECS::GetInstance().Release(mEntity);
}

void Enemy::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();
	
	Attack(deltaTime);
	Move(deltaTime);

	//?
	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
}
void Enemy::Move(float _deltaTime)
{
	transformSystem::MoveForward(mTransform, mSpeed * _deltaTime);
	//mTransform.position.x += mSpeed * _deltaTime;
}
void Enemy::Attack(float _deltaTime)
{
	mCurrentShootCooldown += _deltaTime;
	if (mCurrentShootCooldown >= mShootCooldown) {
		canShoot = true;
		mCurrentShootCooldown = 0;
	}
}


void Enemy::LookAt(Entity target)
{
	transformComponent& targetTransform = ECS::GetInstance().getComponent<transformComponent>(target);

	transformSystem::LookAt(mTransform, targetTransform.position);

}

