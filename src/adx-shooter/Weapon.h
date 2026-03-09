#pragma once
#include "adx-engine/framework.h"

class Weapon
{
	public:
	Weapon() {}
	virtual ~Weapon() {}
	virtual void Attack() = 0;
	virtual void Update() = 0;
protected:
	float Substat;
	float mAttackSpeed;
	float mRange;
	float mRarity;
	float mBaseDamage;
};
