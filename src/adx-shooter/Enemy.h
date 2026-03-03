#pragma once
#include "adx-engine/framework.h"
class Enemy
{
public:
	Entity* mp_player;


	int m_healthPoints = 100; //probs add health regen


	void Update(); //mbe

	void takeDamage(int amount);
	void Shoot();
};