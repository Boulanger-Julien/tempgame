#pragma once
#include "adx-engine/framework.h"
#include "Shoot/Shoot_Pattern.h"
class Boss
{
	public:
	Boss();
	~Boss();
	virtual void Update() = 0;
	virtual void init() = 0;
	virtual void TakeDamage(float damage) = 0;
	bool IsAlive(){
		if (mHealthComponent.mHealth <= 0)
		{
			isAlive = false;
		}
		return isAlive;
	}
	transformComponent& GetTransform(){return mTransform;}
	Entity GetEntity() const{return mEntity;}
	HealthComponent& GetHealthComponent(){return mHealthComponent;}
	virtual void InitStats() = 0;
	virtual void Move(float _deltaTime) = 0;
	virtual void Attack(float _deltaTime) = 0;
	virtual void LookPlayer() = 0;
	float GetStrength() { return mStats.mStrength; }
	float GetHealth() { return mHealthComponent.mHealth; }
	float GetMaxHealth() { return mStats.mHealth; }
	virtual void UpdateComponent() = 0;
	virtual void ChangeShootPattern() = 0;
	std::string GetName() { return name; }
	float ExpToGive() { return (mStats.mExp); }
protected:

	enum BossState
	{
		IDLE,
		MOVING,
		ATTACKING,
		DYING
	};
	BossState currentBossState = IDLE;


	Entity mEntity;
	Entity mHeadEntity;
	ColliderComponent mCollider;
	transformComponent mTransform;
	transformComponent mHeadTransform;
	HealthComponent mHealthComponent;
	StatsComponent mStats;
	bool isAlive = true;
	int mPlayerIndex;
	float mShootCooldown = 1;
	float mCurrentShootCooldown = 0;
	bool canShoot = false;
	int mCurrentShootPattern = 0;
	float timeSinceLastPatternChange = 0;
	float patternChangeInterval = 15.0f;
	int mPatternChangeCooldown = 3;
	float mPatternChangeCurrentCooldown = 0;
	std::string name = "Boss";

	friend struct DyingBossState;
};