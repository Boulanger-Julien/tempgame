#pragma once
#include "adx-engine/framework.h"
#include "adx-core/d3dUtil.h"


class Bullet
{
public:
	std::unordered_map<int, bool > entitiesToIgnore;
	Entity mEntity;
	ColliderComponent mCollider;
	transformComponent mTransform;
	float m_speed = 50;
	float maxLifetime = 1;			// in Seconds
	float currentLifetime = 0;
	bool toBeDestroyed = false;
	int mDamage;
	bool isPersistantBullet = false;
	bool isBoucingBullet = false;
	int allowedBounces = 0;

	Bullet();
	~Bullet();

	void Update();
	void UpdateComponent();
};

