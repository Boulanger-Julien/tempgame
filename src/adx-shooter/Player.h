#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Weapon.h"
#include "Health.h"
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
	void ChangeAimType() {
		if (aimType == AimType::Auto) {
			aimType = AimType::Mouse;
		}
		else {
			aimType = AimType::Auto;
		}
	}
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
	StatsComponent& GetStats() { return mStats; }
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	float GetHealth() { return mHealthComponent.mHealth; }
	int aimType = AimType::Mouse;
private:

	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
};

