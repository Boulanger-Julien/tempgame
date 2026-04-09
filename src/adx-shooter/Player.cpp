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

	mStats.SetStats(100, 0, 3, 0, 20, 0);
	mHealthComponent.mMaxHealth = mStats.mHealth;
	mHealthComponent.mHealth = mStats.mHealth;
	
	mPointsToAllocate = ECS::GetInstance().createEntity(transformComponent(450, 790, 0, 1420, 220));
	UIRenderer healthBarExtMesh(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
	healthBarExtMesh.UIQuad.isRendered = false;
	GameManager::GetInstance().mUIMesh.insert({ mPointsToAllocate, healthBarExtMesh.UIQuad });
	healthBarExtMesh.AddIndex(mPointsToAllocate);
	healthBarExtMesh.PushIndex();

	int offsetX = 525;
	int sizeX = 259;
	HPBar = ECS::GetInstance().createEntity(transformComponent(offsetX, 860, 0, sizeX, 70));
	offsetX += sizeX;
	HPRBar = ECS::GetInstance().createEntity(transformComponent(offsetX, 860, 0, sizeX, 70));
	offsetX += sizeX;
	STRBar = ECS::GetInstance().createEntity(transformComponent(offsetX, 860, 0, sizeX, 70));	
	offsetX += sizeX;
	DEFBar = ECS::GetInstance().createEntity(transformComponent(offsetX, 860, 0, sizeX, 70));	
	offsetX += sizeX;
	SPDBar = ECS::GetInstance().createEntity(transformComponent(offsetX, 860, 0, sizeX, 70));

	InitUI(HPBar, {0,1,0,1});
	InitUI(HPRBar, {0.5f,0.5f,0,1});
	InitUI(STRBar, {1,0,0,1});
	InitUI(DEFBar, {0,0,1,1});
	InitUI(SPDBar, {1,1,0,1});
	
	ChangePattern(1, ShootPatternType::Single_Shot);
	ChangePattern(2, ShootPatternType::None);
	ChangePattern(3, ShootPatternType::None);
}
void Player::InitUI(int index, FLOAT4 color)
{
	UIRenderer barMesh(toXMFLOAT4(color));
	barMesh.AddIndex(index);
	barMesh.PushIndex();
	barMesh.UIQuad.isRendered = false;
	GameManager::GetInstance().mUIMesh.insert({ index, barMesh.UIQuad });
}

void Player::EnableShooting(int shotindex, int pattern)
{


	switch (shotindex) {
	case ShootPatternType::None:
		break;
	case ShootPatternType::Single_Shot:
		if (mNextFShootTimer > mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddBullet();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer > mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddBullet();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer > mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddBullet();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Pump:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddPumpBullet();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddPumpBullet();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddPumpBullet();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Explosion:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddExplosionBullet();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddExplosionBullet();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddExplosionBullet();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Line:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddLineBullet();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddLineBullet();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddLineBullet();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Choc:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddChoc();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddChoc();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddChoc();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Thunder:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddLighting();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddLighting();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddLighting();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Boomerang:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddWindBoomerang();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddWindBoomerang();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddWindBoomerang();
			mNextTShootTimer = 0;
		}
		break;
	case ShootPatternType::Bomb:
		if (mNextFShootTimer >= mFShootColdown && mFShootColdown != 0 && pattern == ShootPatternAttack[1]) {
			AddBomb();
			mNextFShootTimer = 0;
		}
		else if (mNextSShootTimer >= mSShootColdown && mSShootColdown != 0 && pattern == ShootPatternAttack[2]) {
			AddBomb();
			mNextSShootTimer = 0;
		}
		else if (mNextTShootTimer >= mTShootColdown && mTShootColdown != 0 && pattern == ShootPatternAttack[3]) {
			AddBomb();
			mNextTShootTimer = 0;
		}
		break;
	default:
		break;
	}
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
	LevelUp();
}
void Player::takeDamage(int _damage) {
	int dmg = _damage;
	if (mStats.mDefense > 0)
	{
		 dmg -= rand() % (int)mStats.mDefense;
	}
	HealthSystem::TakeDamage(mHealthComponent, _damage-mStats.mDefense);
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
void Player::AddExplosionBullet()
{
	Shot* newShot = Shoot_Pattern_Explosion::Shoot(mEntity, 8, mStats.mStrength, 1, 50);
	for (int i = 0; i < newShot->bulletList.size(); ++i)
	{
		GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[i]->mEntity, GameManager::GetInstance().mBulletMesh });
		GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newShot->bulletList[i]->mEntity);
		GameManager::GetInstance().mPlayerbulletList.push_back(newShot->bulletList[i]);
	}
}
void Player::AddPumpBullet() {
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
	static bool cDownLastFrame4 = false;
	mNextFShootTimer += Timer::GetDeltatime()*2;
	mNextSShootTimer += Timer::GetDeltatime()*1.5f;
	mNextFShootTimer += Timer::GetDeltatime();

	if (InputSystem::isKeyDown(VK_LBUTTON))
	{
		EnableShooting(mFirstShootPattern, ShootPatternAttack[1]);
	}

	if (InputSystem::isKeyDown(VK_RBUTTON))
	{
		EnableShooting(mSecondShootPattern, ShootPatternAttack[2]);
	}

	if (InputSystem::isKeyDown(VK_SPACE)) {
		EnableShooting(mThirdShootPattern, ShootPatternAttack[3]);
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
void Player::LevelUp()
{
	if (mStats.mExp >= mStats.mExpToNextLevel) {
		mStats.mLevel++;
		mStats.mExp -= mStats.mExpToNextLevel;
		mStats.mExpToNextLevel *= mStats.mLevelUpMultiplier; 
		mHealthComponent.mMaxHealth = mStats.mHealth;
		mHealthComponent.mHealth = mStats.mHealth;
		mStats.mStatsPointsToAllocate += 5;
	}
	CanAllocatePoints();
}

void Player::CanAllocatePoints()
{
	static bool mDownLastFrame = false;
	static bool mDownLastFrame2 = false;
	if (InputSystem::isKeyDown('M') && !mDownLastFrame) {
		GameManager::GetInstance().mUIMesh[mPointsToAllocate].isRendered = !GameManager::GetInstance().mUIMesh[mPointsToAllocate].isRendered;
		GameManager::GetInstance().mUIMesh[HPBar].isRendered = !GameManager::GetInstance().mUIMesh[HPBar].isRendered;
		GameManager::GetInstance().mUIMesh[HPRBar].isRendered = !GameManager::GetInstance().mUIMesh[HPRBar].isRendered;
		GameManager::GetInstance().mUIMesh[STRBar].isRendered = !GameManager::GetInstance().mUIMesh[STRBar].isRendered;
		GameManager::GetInstance().mUIMesh[DEFBar].isRendered = !GameManager::GetInstance().mUIMesh[DEFBar].isRendered;
		GameManager::GetInstance().mUIMesh[SPDBar].isRendered = !GameManager::GetInstance().mUIMesh[SPDBar].isRendered;
		mDownLastFrame = true;
	}
	else if (!InputSystem::isKeyDown('M')) {
		mDownLastFrame = false;
	}
	if (mStats.mStatsPointsToAllocate > 0 && GameManager::GetInstance().mUIMesh[mPointsToAllocate].isRendered)
	{
		if (InputSystem::isKeyDown())
		{
			if (!mDownLastFrame2)
			{
				mDownLastFrame2 = true;
				char key = InputSystem::GetKeyDown();
				switch (key)
				{
				case 'G':
					mStats.mHealth += 10;
					mHealthComponent.mMaxHealth = mStats.mHealth;
					mHealthComponent.mHealth = mStats.mHealth;
					mStats.mStatsPointsToAllocate -= 1;
					break;
				case 'H':
					mStats.mHealthRegen += 0.1f;
					mStats.mStatsPointsToAllocate -= 1;
					break;
				case 'J':
					mStats.mStrength += 2;
					mStats.mStatsPointsToAllocate -= 1;
					break;
				case 'K':
					mStats.mDefense += 1;
					mStats.mStatsPointsToAllocate -= 1;
					break;
				case 'L':
					mStats.mSpeed += 2;
					mStats.mStatsPointsToAllocate -= 1;
					break;
				default:
					return;
				}
			}
		}
		else
		{
			mDownLastFrame2 = false;
		}
	}
}

void Player::ChangePattern(int key, int pattern)
{
	switch (key) {
	case 1:
		mFirstShootPattern = pattern;
		ShootPatternAttack[1] = mFirstShootPattern;
		mFShootColdown = ShootPatternCooldowns[pattern];
		break;
	case 2:
		mSecondShootPattern = pattern;
		ShootPatternAttack[2] = mSecondShootPattern;
		mSShootColdown = ShootPatternCooldowns[pattern];
		break;
	case 3:
		mThirdShootPattern = pattern;
		ShootPatternAttack[3] = mThirdShootPattern;
		mTShootColdown = ShootPatternCooldowns[pattern];
		break;
	default:
		break;
	}
}


