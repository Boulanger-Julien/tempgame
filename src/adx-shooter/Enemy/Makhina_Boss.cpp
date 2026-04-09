#include "pch.h"
#include "Makhina_Boss.h"
#include "GameManager.h"
#include "State.h"
Makhina_Boss::~Makhina_Boss()
{
}

void Makhina_Boss::Update()
{
	float deltatime = Timer::GetInstance()->GetDeltatime();



	switch (currentBossState)
	{
	case BossState::IDLE:
		LookPlayer();
		Attack(deltatime);
		ChangeShootPattern();
		break;
	case BossState::DYING:
		DyingBossState::OnExecute(*this);
		break;
	default:
		break;
	}
	UpdateComponent();
}

void Makhina_Boss::init()
{
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 0, 0, 10, 10, 10), ColliderComponent(), HealthComponent());
	mHeadEntity = ECS::GetInstance().createEntity(transformComponent(0, 0, 0, 1, 1, 1));
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mHeadTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mHeadTransform.position.y = 2.0f;
	mHealthComponent = ECS::GetInstance().getComponent<HealthComponent>(mEntity);
	mCollider.compOwner = mEntity;
	mCollider.updateCollider();
	mStats.SetStats(300, 0, 30, 10, 0, 60);
	mStats.mExp = 1000;
	mHealthComponent.mHealth = mStats.mHealth;
	name = "Makhina, Creation Promise";
}

void Makhina_Boss::Attack(float _deltaTime)
{
	if (mPatternChangeCurrentCooldown >= mPatternChangeCooldown)
	{
		if (mCurrentShootCooldown <= 0) {

			switch (mCurrentShootPattern)
			{
			case 1:
			{
				Bullet* newBullet = Shoot_Pattern_Single_Shot::Shoot(mHeadEntity, 2,100,85, mStats.mStrength);
				GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newBullet->mEntity);
				GameManager::GetInstance().mEntityMesh.insert({ newBullet->mEntity, GameManager::GetInstance().mBulletMesh });
				XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity));
				GameManager::GetInstance().GetWindow()->Update(newBullet->mEntity, bulletWorld);
				GameManager::GetInstance().mBulletList.push_back(newBullet);
				mCurrentShootCooldown = mShootCooldown;
				break;
			}

			case 2:
			{
				Shot* newShot = Shoot_Pattern_Explosion::Shoot(mHeadEntity, 9, mStats.mStrength, 2,100);
				for (int i = 0; i < newShot->bulletList.size(); ++i)
				{
					GameManager::GetInstance().mEntityMesh.insert({ newShot->bulletList[i]->mEntity, GameManager::GetInstance().mBulletMesh });
					GameManager::GetInstance().mBulletList.push_back(newShot->bulletList[i]);
					GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newShot->bulletList[i]->mEntity);

				}
				mCurrentShootCooldown = mShootCooldown;
				break;
			}

			case 3:
			{
				Bullet* newLineBullet = Shoot_Pattern_Line::Shoot(mHeadEntity, mStats.mStrength, 25, 1);
				GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newLineBullet->mEntity);
				GameManager::GetInstance().mEntityMesh.insert({ newLineBullet->mEntity, GameManager::GetInstance().mLineBulletMesh });
				XMMATRIX lineBulletWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newLineBullet->mEntity));
				GameManager::GetInstance().GetWindow()->Update(newLineBullet->mEntity, lineBulletWorld);
				newLineBullet->mDamage = mStats.mStrength;
				GameManager::GetInstance().mBulletList.push_back(newLineBullet);
				mCurrentShootCooldown = mShootCooldown;
				break;
			}
			case ShootPatternType::None:
				break;
			default:
				break;
			}
		}
		else {
			mCurrentShootCooldown -= _deltaTime;
		}
	}

}

void Makhina_Boss::ChangeShootPattern()
{
	static bool firstFrame = true;
	mPatternChangeCurrentCooldown += Timer::GetInstance()->GetDeltatime();
	if (mPatternChangeCurrentCooldown >= mPatternChangeCooldown || firstFrame)
	{
		timeSinceLastPatternChange += Timer::GetInstance()->GetDeltatime();
		if (timeSinceLastPatternChange >= patternChangeInterval || firstFrame)
		{
			firstFrame = false;
			timeSinceLastPatternChange = 0;
			mCurrentShootPattern = rand() % 4;
			switch (mCurrentShootPattern)
			{
			case 1://single shot pattern
				mShootCooldown = 0.2f;
				mCurrentShootCooldown = 0;
				mStats.mStrength = 30;
				mPatternChangeCurrentCooldown = 0;
				patternChangeInterval = 15.0f;
				break;
			case 2: //explosion pattern
				mShootCooldown = 0.5;
				mCurrentShootCooldown = 0;
				mStats.mStrength = 50;
				mPatternChangeCurrentCooldown = 0;
				patternChangeInterval = 10.0f;
				break;
			case 3: //line pattern
				mShootCooldown = 0.1f;
				mCurrentShootCooldown = 0;
				mStats.mStrength = 60;
				mPatternChangeCurrentCooldown = 0;
				patternChangeInterval = 10.0f;
				break;
			default:
				mCurrentShootPattern = ShootPatternType::None; 
				mShootCooldown = 10;
				mCurrentShootCooldown = 0;
				mStats.mStrength = 0;
				mPatternChangeCurrentCooldown = 0;
				patternChangeInterval = 5.0f;
				break;
			}
		}
	}
}


