#include "pch.h"
#include "Player.h"
#include "adx-core/Timer.h"
#include "adx-engine\framework.h"
#include "Weapon/Basic_Sword.h"
#include "GameManager.h"
#include "LimitMapSystem.h"

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

	mStats.SetStats(100, 2, 0, 0, 40, 0, 35, 0, 0);
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
	mousePos = {targetX, targetZ };
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
	Shoot();
	MoveByKey();
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

	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity, 1, 50, (aimType == AimType::Mouse ? 100:85), mStats.mStrength);
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);
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
				mousePos = { enemyTrans.position.x, enemyTrans.position.z };
			}
		}

		for (Enemy* enemy : GameManager::GetInstance().mEnemyList) {
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
				mousePos = { enemyTrans.position.x, enemyTrans.position.z };
			}
		}
	}
}
void Player::AddLineBullet() {

	Bullet* newBullet = Shoot_Pattern_Line::Shoot(mEntity, mStats.mStrength, 25,1);
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);

	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mLineBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);

	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
}


void Player::AddExplosionBullet() {
	Shot* newShot = Shoot_Pattern_Pump::Shoot(mEntity, 9, mStats.mStrength, 1, 50);
	for (int i = 0; i < newShot->bulletList.size(); ++i)
	{
		GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[i]->mEntity, GameManager::GetInstance().mBulletMesh });
		GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newShot->bulletList[i]->mEntity);
		GameManager::GetInstance().mPlayerbulletList.push_back(newShot->bulletList[i]);
	}
}

void Player::AddLighting()
{
	
	Shot* newShot = Shoot_Pattern_Thunder::Shoot(mEntity, mStats.mStrength*4,{mousePos.x,0,mousePos.y});
	GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[0]->mEntity, GameManager::GetInstance().mCircleMesh });
	GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[1]->mEntity, GameManager::GetInstance().mLineBulletMesh });
	for (int i = 0; i < newShot->bulletList.size(); ++i)
	{
		GameManager::GetInstance().mPlayerbulletList.push_back(newShot->bulletList[i]);
		GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newShot->bulletList[i]->mEntity);
	}
}

void Player::AddChoc() {
	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity, 1, 75, (aimType == AimType::Mouse ? 100 : 85), mStats.mStrength);
	newBullet->isPersistantBullet = true;
	newBullet->isBoucingBullet = true;
	newBullet->allowedBounces = 4;
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);
	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
}

void Player::AddBomb()
{
	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity, 1, 25, (aimType == AimType::Mouse ? 100 : 85), mStats.mStrength*3);
	newBullet->isBomb = true;
	newBullet->bombDamage = mStats.mStrength;
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);
	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
}

void Player::AddWindBoomerang()
{
	Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mEntity, 1, 40, (aimType == AimType::Mouse ? 100 : 85), mStats.mStrength * 1.5f);
	newBullet->isWind = true;
	newBullet->isPersistantBullet = true;
	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mThorusMesh });
	XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);
	GameManager::GetInstance().mPlayerbulletList.push_back(newBullet);
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
	case 3:
		AddLighting();
		break;
	case 4:
		AddChoc();
		break;
	case 5:
		AddBomb();
	case 6:
		AddWindBoomerang();
		break;
	default:
		break;
	}
	patternIndex = (patternIndex + 1) % (ShootPatternType::Amount - 1);
}

void Player::Shoot()
{
	static bool cDownLastFrame = false;
	static bool cDownLastFrame2 = false;
	static bool cDownLastFrame3 = false;
	static bool cDownLastFrame4 = false;
	if (InputSystem::isKeyDown(VK_LBUTTON))
	{
		if (!cDownLastFrame) {
			AddWindBoomerang();
			cDownLastFrame = true;
		}
	}
	else {
		cDownLastFrame = false;
	}
	if (InputSystem::isKeyDown(VK_SPACE)) {
		if (!cDownLastFrame2) {
			AddChoc();
			cDownLastFrame2 = true;
		}
	}
	else {
		cDownLastFrame2 = false;
	}
	if (InputSystem::isKeyDown(VK_RBUTTON))
	{
		if (!cDownLastFrame3) {
			AddLineBullet();
			cDownLastFrame3 = true;
		}
	}
	else {
		cDownLastFrame3 = false;
	}
	if (InputSystem::isKeyDown('W')) {
		if (!cDownLastFrame4) {
			TestShootPattern();
			cDownLastFrame4 = true;
		}
	}
	else {
		cDownLastFrame4 = false;
	}

}

void Player::MoveByKey()
{
	float dt = Timer::GetInstance()->GetDeltatime();
	int velo = mStats.mSpeed;
	// On convertit l'angle de degrés en radians si nécessaire
	// float rad = angle.y * (3.14159f / 180.0f); 
	float rad = -45 * XM_PI / 180.0f;

	// Calcul des vecteurs forward (Z) et right (X)
	float forwardX = sin(rad) * dt * velo;
	float forwardZ = cos(rad) * dt * velo;

	float rightX = cos(rad) * dt * velo;
	float rightZ = -sin(rad) * dt * velo;

	transformComponent newTransform = mTransform;
	// Z / S : Avancer / Reculer
	if (InputSystem::isKeyDown('Z'))
		transformSystem::Move(newTransform, forwardX, 0, forwardZ);
	if (InputSystem::isKeyDown('S'))
		transformSystem::Move(newTransform, -forwardX, 0, -forwardZ);

	// Q / D : Strafe Gauche / Droite
	if (InputSystem::isKeyDown('Q'))
		transformSystem::Move(newTransform, -rightX, 0, -rightZ);
	if (InputSystem::isKeyDown('D'))
		transformSystem::Move(newTransform, rightX, 0, rightZ);

	if (!LimitMapSystem::CheckLimitMap(newTransform, GameManager::GetInstance().currentRoom))
		mTransform = newTransform;

	return;
}
