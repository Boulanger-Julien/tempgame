#pragma once
#include "adx-engine/framework.h"

class Enemy
{
private:
	int m_shootCooldown = 1;
	float m_currentShootCooldown = 0;
	transformComponent mTransform;
	ColliderComponent mCollider;
	int mPlayerIndex;
public:
	Entity m_entity;

	bool canShoot = false;

	bool isDead = false;
	Enemy(int _playerIndex);
	~Enemy();
	void Update(); 
	void LookAt(Entity target);
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }
};