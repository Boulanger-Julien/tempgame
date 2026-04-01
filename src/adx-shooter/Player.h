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
	void TestShootPattern();
	void CheckInput();
	void SetMesh(Mesh* mesh) { mRender.mesh = mesh; }
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
	StatsComponent& GetStats() { return mStats; }
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	RenderComponent& GetRender() { return mRender; }
	float GetHealth() { return mHealthComponent.mHealth; }
	int aimType = AimType::Mouse;

	float mShootColdown = 0.4;
	float mNextShootTimer = 0;
private:

	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
	RenderComponent mRender;
};

