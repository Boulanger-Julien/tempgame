#pragma once
#include "adx-engine/framework.h"

class Player
{
private:
public:
	StatsComponent Stats;
	Entity m_entity;

	Player();
	void ChooseClass(int classID);
	void Update();

	void takeDamage(int damage);
};

