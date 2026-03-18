#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Weapon.h"
enum AimType {
	Auto,
	Mouse
};

class Player
{
public:
	Entity m_entity;
	Weapon* equippedWeapon;
	Player();
	void ChooseClass(int classID);
	void Update(const Ray& mouseRay);
	void Update();
	void takeDamage(int damage);
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
	StatsComponent& GetStats() { return Stats; }
	int aimType = AimType::Mouse;
private:
	transformComponent mTransform;
	ColliderComponent mCollider;
	StatsComponent Stats;
};

