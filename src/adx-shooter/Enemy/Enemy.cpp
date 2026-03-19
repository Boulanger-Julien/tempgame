#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"

Enemy::Enemy(int _playerIndex) {
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 2, 0), ColliderComponent(), StatsComponent(), HealthComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);

	mStats = ECS::GetInstance().getComponent<StatsComponent>(mEntity);
	mHealthComponent = ECS::GetInstance().getComponent<HealthComponent>(mEntity);

	mCollider.depth = mTransform.scale.z * 2;
	mCollider.width = mTransform.scale.x * 2;
	mCollider.height = mTransform.scale.y * 2;

	mCollider.compOwner = mEntity;
	mCollider.updateCollider();
	mPlayerIndex = _playerIndex;

	InitStats();
}

Enemy::~Enemy() {
	ECS::GetInstance().Release(mEntity);
}

void Enemy::InitStats()
{
	mStats.SetStats(100, 0, 0, 0, 5, 0, 22, 0, 0);
	mHealthComponent.mHealth = mStats.mHealth;
}

void Enemy::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();
	
	float dist = transformSystem::GetDistance(mTransform, ECS::GetInstance().getComponent<transformComponent>(mPlayerIndex));
	if (dist <= 60) {
		LookPlayer();
		if (dist >= 35) {
			Move(deltaTime);
		}
		if (dist <= 40) {
			Attack(deltaTime);
		}
	}

	//Toujours � la fin
	UpdateComponent();
}
void Enemy::Move(float _deltaTime)
{
	transformSystem::MoveForward(mTransform, mStats.mSpeed * _deltaTime);
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
	HealthSystem::TakeDamage(mHealthComponent, _damage);
}

bool Enemy::IsAlive()
{
	if (GetHealth() <= 0) {
		isDead = true;
	}

	return !isDead;
}

void Enemy::LookPlayer()
{
	//transformComponent& targetTransform = mPlayer->GetTransform();

	transformSystem::LookAt(mTransform, ECS::GetInstance().getComponent<transformComponent>(mPlayerIndex).position);

}

void Enemy::UpdateComponent()
{
	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
	if (mHealthComponent.mHealth > mStats.mHealth) {
		mHealthComponent.mHealth = mStats.mHealth;
	}
	ECS::GetInstance().getComponent<HealthComponent>(mEntity) = mHealthComponent;
}

