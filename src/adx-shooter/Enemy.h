#pragma once
#include "adx-engine/framework.h"
#include "Player.h"

class Enemy
{
private:
	int mShootCooldown = 1;
	float mCurrentShootCooldown = 0;
	transformComponent mTransform;
	ColliderComponent mCollider;

	int mPlayerIndex;
public:
	Entity mEntity;
	Player* mPlayer;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
	//TO DO : voir ou est include HealthComponent

	bool canShoot = false;
	bool isDead = false;
	Enemy(int _playerIndex);
	~Enemy();
	void InitStats();
	void Update(); 

	void Move(float _deltaTime);
	void Attack(float _deltaTime);
	void TakeDamage(int _damage);
	bool IsAlive();

	void LookPlayer();

	void UpdateComponent();
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }

	float GetStrength() { return mStats.mStrength; }
	float GetHealth() { return mHealthComponent.mHealth; }
};