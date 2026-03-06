#pragma once
#include "adx-engine/framework.h"

class Player
{
private:
	int m_healthRegenCooldown = 1;
	float m_currentHealthRegenCooldown = 0;

	float m_shootCooldown = 0.5f;
	float m_currentShootCooldown = 0;
	float m_timeSpeed = 1.0f;

public:
	Entity m_entity;

	int m_score = 0;
	float m_healthPoints = 100;
	float m_maxHealthPoints = 100;
	bool canShoot = true;

	bool isCrouching = false;
	float baseY;

	Player();
	void Update();

	void takeDamage();
	void SlowDown(bool slow);
};

