#pragma once
#include "adx-engine/framework.h"

class Player
{
private:
	int mHealthRegenCooldown = 5.0f;
	float mCurrentHealthRegenCooldown = 0;
	int mHPRegenAmount = 1;

public:
	Entity m_entity;

	int mExp = 0;
	float mHealthPoints = 100;
	float mMaxHealthPoints = 100;

	Player();
	void Update();

	void takeDamage(int damage);
};

