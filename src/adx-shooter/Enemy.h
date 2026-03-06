#pragma once
#include "adx-engine/framework.h"

class Enemy
{
private:
	int m_shootCooldown = 1;
	float m_currentShootCooldown = 0;

public:
	Entity m_entity;

	bool canShoot = false;

	Enemy();
	~Enemy();
	void Update(); 
	void LookAt(Entity target);
};