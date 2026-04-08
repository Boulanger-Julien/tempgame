#pragma once
#include "adx-engine/framework.h"
#include "Player.h"
#include "adx-render/UIRenderer.h"

class Enemy
{
protected:
	float distFollowPlayer = 0;
	float distAttackPlayer = 0;
	float mShootCooldown = 1;
	int mPlayerIndex;

	virtual void OnUpdate(float _deltaTime);
	virtual void OnInit();

	bool CheckDistanceToFollowPlayer();
	bool CheckDistanceToAttackPlayer();
	void InitStats(float _health, float _healthRegen, float _strength, float _defense, float _moveSpeed, float _exp);
	enum EnemyState {
		NONE,
		FOLLOWPLAYER,
		ATTACKPLAYER,
		STUNTED
	};
	EnemyState currentEnemyState = NONE;
private:
	int shootPatternIndex = 0;
	float mCurrentShootCooldown = 0;
	transformComponent mTransform;
	ColliderComponent mCollider;


public:
	Entity mEntity;
	Player* mPlayer;
	StatsComponent mStats;
	HealthComponent mHealthComponent;
	Entity healthBar;
	//TO DO : voir ou est include HealthComponent

	bool canShoot = false;
	bool isDead = false;

	void Init(int _playerIndex);
	void Update();

	void MoveTowardPlayer(float _deltaTime);
	virtual void Attack(float _deltaTime);
	void AddBullet();
	void ExplosionBullets();
	void LineBullets();
	void TakeDamage(int _damage);
	bool IsAlive();

	void LookPlayer();

	void UpdateComponent();
	HealthComponent& GetHealthComponent() { return mHealthComponent; }
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }

	float GetStrength() { return mStats.mStrength; }
	float GetHealth() { return mHealthComponent.mHealth; }
	Entity GetEntity() const { return mEntity; }


};