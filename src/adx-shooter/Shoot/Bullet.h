#pragma once
#include "adx-engine/framework.h"
#include "adx-core/d3dUtil.h"


enum BulletType
{
	BULLET,
	LINE
};
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

	BulletType type = BULLET;
	Bullet();
	~Bullet();

	void Update();
	void UpdateComponent();
};

