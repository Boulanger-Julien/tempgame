#pragma once
#include "adx-engine/framework.h"
class PlayerController{};

class Player
{
public:
	Entity mp_entity;
	PlayerController* mp_controller; 

	int m_healthPoints = 100; //probs add health regen

	Player();
	void Update(); //mbe

	void takeDamage(int amount);
	void Shoot();
};

