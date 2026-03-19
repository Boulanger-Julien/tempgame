#pragma once
#include "adx-engine/framework.h"
#include "adx-core/d3dUtil.h"


class Bullet
{
public:
	Entity mEntity;
	ColliderComponent mCollider;
	transformComponent mTransform;
	float m_speed = 50;
	float maxLifetime = 1;			// in Seconds
	float currentLifetime = 0;
	bool toBeDestroyed = false;
	int mDamage;
	bool isLineBullet = false;

	Bullet();
	~Bullet();

	void Update();
	void UpdateComponent();
};

