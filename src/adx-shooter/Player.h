#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Weapon/Weapon.h"
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
	void AddLighting();
	void AddChoc();
	void AddBomb();
	void AddWindBoomerang();
	void TestShootPattern();
	void Shoot();
	void MoveByKey();
	void LevelUp();
	void CanAllocatePoints();
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
	FLOAT2 mousePos;
	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
	void InitUI(int index, FLOAT4 color);
};

