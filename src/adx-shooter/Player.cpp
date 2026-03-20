#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"
#include "adx-engine\framework.h"
#include "Weapon/Basic_Sword.h"
#include "GameManager.h"


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

	mStats.SetStats(100, 2, 0, 0, 20, 0, 35, 0, 0);
	mHealthComponent.mMaxHealth = mStats.mHealth;
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
	HealthSystem::RecoverHealth(mHealthComponent, mStats.mHealthRegen * _deltatime);

	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	transformComponent weaponTransform = mTransform;
	transformSystem::RotateAround(weaponTransform, mTransform, 1.5f);
	ECS::GetInstance().getComponent<transformComponent>(mWeapon->GetEntity()) = weaponTransform;
	transformSystem::MoveByKey(mTransform, mStats.mSpeed, -45, _deltatime);
}
void Player::takeDamage(int _damage) {
	HealthSystem::TakeDamage(mHealthComponent, _damage);
}
void Player::ChangeAimType()
{
	if (aimType == AimType::Auto) {
		aimType = AimType::Mouse;
	}
	else {
		aimType = AimType::Auto;
	}
}
void Player::AddBullet() {

	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity);
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);

	newBullet->mDamage = mStats.mStrength;
	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
}

void Player::Aim()
{
	if (aimType == AimType::Mouse)
	{
		POINT mousePos = { (LONG)InputSystem::GetMouseX(), (LONG)InputSystem::GetMouseY() };
		ScreenToClient(GameManager::GetInstance().GetWindow()->MainWnd(), &mousePos);
		float finalMouseX = static_cast<float>(mousePos.x);
		float finalMouseY = static_cast<float>(mousePos.y);

		Ray ray = GameManager::GetInstance().GetCamera().GetRayFromMouse(finalMouseX, finalMouseY, GameManager::GetInstance().GetWindow()->mWindowRect.right, GameManager::GetInstance().GetWindow()->mWindowRect.bottom);
		Update(ray);
	}
	else if (aimType == AimType::Auto)
	{
		Update();
		float closestDistance = FLT_MAX;
		transformComponent& playerTrans = GetTransform();
		for (Boss* enemy : GameManager::GetInstance().mBossList) {
			transformComponent& enemyTrans = ECS::GetInstance().getComponent<transformComponent>(enemy->GetEntity());
			float distance = sqrt(pow(playerTrans.position.x - enemyTrans.position.x, 2) + pow(playerTrans.position.z - enemyTrans.position.z, 2));
			if (distance < closestDistance) {
				closestDistance = distance;
			}
			else
			{
				continue;
			}
			if (distance < 70) {
				float dx = enemyTrans.position.x - playerTrans.position.x;
				float dz = enemyTrans.position.z - playerTrans.position.z;
				float angle = atan2f(dx, dz);
				playerTrans.rotation.y = angle;
			}
		}

		for (EnemyMarksman* enemy : GameManager::GetInstance().mEnemyList) {
			transformComponent& enemyTrans = ECS::GetInstance().getComponent<transformComponent>(enemy->mEntity);
			float distance = sqrt(pow(playerTrans.position.x - enemyTrans.position.x, 2) + pow(playerTrans.position.z - enemyTrans.position.z, 2));
			if (distance < closestDistance) {
				closestDistance = distance;
			}
			else
			{
				continue;
			}
			if (distance < 70) {
				float dx = enemyTrans.position.x - playerTrans.position.x;
				float dz = enemyTrans.position.z - playerTrans.position.z;
				float angle = atan2f(dx, dz);
				playerTrans.rotation.y = angle;
			}
		}
	}
}
void Player::AddLineBullet() {

	Bullet* newBullet = Shoot_Pattern_Line::Shoot(mEntity, mStats.mStrength, 25,1,GameManager::GetInstance().GetWindow());
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mLineBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);

	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
}


void Player::AddExplosionBullet() {
	Shot* newShot = Shoot_Pattern_Explosion::Shoot(mEntity, 9, mStats.mStrength, GameManager::GetInstance().GetWindow());
	for (int i = 0; i < newShot->bulletList.size(); ++i)
	{
		GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[i]->mEntity, GameManager::GetInstance().mBulletMesh });
		GameManager::GetInstance().mPlayerbulletList.push_back(newShot->bulletList[i]);
	}
}

void Player::TestShootPattern()
{
	static int patternIndex = 0;
	switch (patternIndex) {
	case 0:
		AddBullet();
		break;
	case 1:
		AddExplosionBullet();
		break;
	case 2:
		AddLineBullet();
		break;
	default:
		break;
	}
	patternIndex = (patternIndex + 1) % (ShootPatternType::Amount - 1);
}