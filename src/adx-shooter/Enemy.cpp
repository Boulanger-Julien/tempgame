#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"

Enemy::Enemy(Player* _player) {
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 2, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mCollider.depth = mTransform.scale.z * 2;
	mCollider.width = mTransform.scale.x * 2;
	mCollider.height = mTransform.scale.y * 2;

	mCollider.compOwner = mEntity;
	mCollider.updateCollider();

	mPlayer = _player;
}

Enemy::~Enemy() {
	ECS::GetInstance().Release(mEntity);
}

void Enemy::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();
	
	float dist = transformSystem::GetDistance(mTransform, mPlayer->GetTransform());
	if (dist <= 60) {
		LookPlayer();
		if (dist >= 35) {
			Move(deltaTime);
		}
		if (dist <= 40) {
			Attack(deltaTime);
		}
	}

	//Toujours à la fin
	UpdateComponent();
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
void Enemy::TakeDamage(int _damage) {
	mHealth -= _damage;
}

bool Enemy::IsAlive()
{
	if (mHealth <= 0) {
		isDead = true;
	}

	return isDead;
}

void Enemy::LookPlayer()
{
	//transformComponent& targetTransform = mPlayer->GetTransform();

	transformSystem::LookAt(mTransform, mPlayer->GetTransform().position);

}

void Enemy::UpdateComponent()
{
	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
}

