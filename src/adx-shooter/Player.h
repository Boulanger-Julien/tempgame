#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Weapon/Weapon.h"
#include "Shoot/Shoot_Pattern.h"
enum AimType {
	Auto,
	Mouse
};



class Player
{
public:
	Entity mEntity;
	Weapon* mWeapon;
	

	Player();
	void ChooseClass(int classID);
	void Update(const Ray& mouseRay);
	void Update();
	void OnUpdate(float _deltaTime);
	void takeDamage(int _damage);
	void ChangeAimType();
	void Aim();
	void AddBullet();
	void AddLineBullet();
	void AddExplosionBullet();
	void AddPumpBullet();
	void AddLighting();
	void AddChoc();
	void AddBomb();
	void AddWindBoomerang();
	void TestShootPattern();
	void Shoot();
	void MoveByKey();
	void LevelUp();
	void CanAllocatePoints();
	void ChangePattern(int key, int pattern);
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
	StatsComponent& GetStats() { return mStats; }
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	float GetHealth() { return mHealthComponent.mHealth; }
	int aimType = AimType::Mouse;
	Entity mPointsToAllocate;
	Entity HPBar;
	Entity HPRBar;
	Entity STRBar;
	Entity DEFBar;
	Entity SPDBar;
private:
	std::unordered_map<int, float> ShootPatternCooldowns {
		{ ShootPatternType::None, 0.0f },
		{ ShootPatternType::Single_Shot, 0.5f },
		{ ShootPatternType::Explosion, 3.0f },
		{ ShootPatternType::Pump, 2.0f },
		{ ShootPatternType::Line, 0.75f },
		{ ShootPatternType::Choc, 1.5f },
		{ ShootPatternType::Thunder, 5.0f },
		{ ShootPatternType::Boomerang, 3.0f },
		{ ShootPatternType::Bomb, 5.0f } 
	};
	std::unordered_map<int, int> ShootPatternAttack
	{
		{ 1, 1 },
		{ 2, 2 },
		{ 3, 3 }
	};
	FLOAT2 mousePos;
	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
	void InitUI(int index, FLOAT4 color);
	void EnableShooting(int shotindex, int pattern);
	float mFShootColdown = 0.0;
	float mNextFShootTimer = 0;
	float mSShootColdown = 0.0;
	float mNextSShootTimer = 0;
	float mTShootColdown = 0.0;
	float mNextTShootTimer = 0;
	int mFirstShootPattern = 1;
	int mSecondShootPattern = 0;
	int mThirdShootPattern = 0;
};

