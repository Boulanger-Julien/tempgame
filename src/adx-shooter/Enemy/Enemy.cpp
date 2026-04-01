#include "pch.h"
#include "Enemy.h"
#include "adx-core/Timer.h"
#include "GameManager.h"
//
#include "Shoot\Bullet.h"
#include "Shoot\Shoot_Pattern.h"

//Protected
bool Enemy::CheckDistanceToFollowPlayer()
{
	return transformSystem::GetDistance(mTransform, ECS::GetInstance().getComponent<transformComponent>(mPlayerIndex)) <= distFollowPlayer;
}

bool Enemy::CheckDistanceToAttackPlayer()
{
	return transformSystem::GetDistance(mTransform, ECS::GetInstance().getComponent<transformComponent>(mPlayerIndex)) <= distAttackPlayer;
}

//
void Enemy::Init(int _playerIndex)
{
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

	OnInit();
}

void Enemy::InitStats(float _health, float _healthRegen, float _mana, float _manaRegen, float _strength, float _defense, float _moveSpeed, float _exp, float _magicPower)
{
	mStats.SetStats(_health, _healthRegen, _mana, _manaRegen, _strength, _defense, _moveSpeed, _exp, _magicPower);
	mHealthComponent.mHealth = mStats.mHealth;
}

void Enemy::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();
	
	OnUpdate(deltaTime);

	UpdateComponent();
}
void Enemy::OnUpdate(float _deltaTime) {

}
void Enemy::OnInit()
{

}
void Enemy::MoveTowardPlayer(float _deltaTime)
{
	transformSystem::MoveForward(mTransform, mStats.mSpeed * _deltaTime);
	//mTransform.position.x += mSpeed * _deltaTime;
}
void Enemy::Attack(float _deltaTime)
{
	mCurrentShootCooldown += _deltaTime;
	if (mCurrentShootCooldown >= mShootCooldown) {
		AddBullet();
		mCurrentShootCooldown = 0;
	}
	//

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

void Enemy::AddBullet() {

	//Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity);
	//GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	//GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	//XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	//GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);

	//newBullet->mDamage = mStats.mStrength;
	//GameManager::GetInstance().mBulletList.push_back(newBullet);

}