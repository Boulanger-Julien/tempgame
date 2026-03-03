#pragma once
#include "adx-engine/framework.h"
#include "adx-core/d3dUtil.h"


class Bullet
{
public:
	Entity m_entity;

	float m_speed = 5;

	int maxLifetime = 5;			// in Seconds
	float currentLifetime = 0;
	bool toBeDestroyed = false;

	Bullet();
	~Bullet();

	void Update();
};

