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
void Enemy::Init(int _playerIndex, Window* _window)
{
	healthBar = ECS::GetInstance().createEntity(transformComponent(0, 0));
	GameManager::GetInstance().mUIMesh.insert({ healthBar, GameManager::GetInstance().mHealthBarMesh.UIQuad});
	ECS::GetInstance().getComponent<transformComponent>(healthBar).scale.x = 100;
	ECS::GetInstance().getComponent<transformComponent>(healthBar).scale.y = 10;

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
	shootPatternIndex = rand() % 3;
	switch (shootPatternIndex) {
	case 0: // Single Shot
		canShoot = true;
		mShootCooldown = 1;
		break;
	case 1: // Explosion
		canShoot = true;
		mShootCooldown = 3;
		break;
	case 2: // Line
		canShoot = true;
		mShootCooldown = 2;
		break;
	default:
		canShoot = false;
		break;
	}
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
		switch (shootPatternIndex) {
			case 0: // Single Shot
			AddBullet();
			break;
			case 1: // Explosion
				ExplosionBullets();
			break;
			case 2: // Line
				LineBullets();
				break;
			default:
				break;
		}

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
	auto& uiTransform = ECS::GetInstance().getComponent<transformComponent>(healthBar);
	XMFLOAT3 enemyPos = toXMFLOAT3(mTransform.position);
	XMVECTOR screenPos = XMVector3Project(
		XMLoadFloat3(&enemyPos),
		0, 0, GameManager::GetInstance().GetWindow()->mWindowRect.right, GameManager::GetInstance().GetWindow()->mWindowRect.bottom,
		0.0f, 1.0f,
		GameManager::GetInstance().GetCamera().Proj(),
		GameManager::GetInstance().GetCamera().View(),
		XMMatrixIdentity()
	);
	uiTransform.position.x = XMVectorGetX(screenPos) - uiTransform.scale.x/2;
	uiTransform.position.y = XMVectorGetY(screenPos) - 50; 

	float healthRatio = (float)mHealthComponent.mHealth / (float)mStats.mHealth;
	uiTransform.scale.x = 100.0f * healthRatio;
}

void Enemy::AddBullet() {

	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity, 1, 50,85);
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);

	newBullet->mDamage = mStats.mStrength;
	GameManager::GetInstance().mBulletList.push_back(newBullet);

}

void Enemy::ExplosionBullets()
{
	Shot* newShot = Shoot_Pattern_Explosion::Shoot(mEntity, 10, mStats.mStrength, GameManager::GetInstance().GetWindow(), 1, 50);
	for (Bullet* bullet : newShot->bulletList) {
		GameManager::GetInstance().mBulletList.push_back(bullet);
		GameManager::GetInstance().mEntityMesh.insert({ bullet->mEntity, GameManager::GetInstance().mBulletMesh });
	}
}

void Enemy::LineBullets()
{
	Bullet* newLineBullet = Shoot_Pattern_Line::Shoot(mEntity, mStats.mStrength, 10, 1, GameManager::GetInstance().GetWindow());
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newLineBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newLineBullet->mEntity, GameManager::GetInstance().mLineBulletMesh });
	XMMATRIX lineBulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newLineBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newLineBullet->mEntity, lineBulletWorld);
	newLineBullet->mDamage = mStats.mStrength;
	GameManager::GetInstance().mBulletList.push_back(newLineBullet);
}