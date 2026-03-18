#pragma once
#include "adx-engine/framework.h"

class Enemy
{
private:
	int mShootCooldown = 1;
	float mSpeed = 5;
	float mCurrentShootCooldown = 0;
	transformComponent mTransform;
	ColliderComponent mCollider;

public:
	Entity mEntity;

	bool canShoot = false;

	bool isDead = false;
	Enemy();
	~Enemy();

	void Update(); 

	void Move(float _deltaTime);
	void Attack(float _deltaTime);

	void LookAt(Entity target);

	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
};