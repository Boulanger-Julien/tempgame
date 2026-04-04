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
	void TestShootPattern();
	void Shoot();
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
	StatsComponent& GetStats() { return mStats; }
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	float GetHealth() { return mHealthComponent.mHealth; }
	int aimType = AimType::Mouse;
private:
	FLOAT2 mousePos;
	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
};

