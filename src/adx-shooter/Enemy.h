#pragma once
#include "adx-engine/framework.h"
#include "Player.h"

class Enemy
{
private:
	int mHealth = 100;
	int mShootCooldown = 1;
	float mSpeed = 5;
	float mCurrentShootCooldown = 0;
	transformComponent mTransform;
	ColliderComponent mCollider;
	float mStrength = 5;

	int mPlayerIndex;
public:
	Entity mEntity;
	Player* mPlayer;

	bool canShoot = false;
	bool isDead = false;
	Enemy(int _playerIndex);
	~Enemy();

	void Update(); 

	void Move(float _deltaTime);
	void Attack(float _deltaTime);
	void TakeDamage(int _damage);
	bool IsAlive();

	void LookPlayer();

	void UpdateComponent();

	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }

	float GetStrength() { return mStrength; }
};