#pragma once
#include "adx-engine/framework.h"

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
	virtual void UpdateComponent() = 0;
protected:
	Entity mEntity;
	ColliderComponent mCollider;
	transformComponent mTransform;
	HealthComponent mHealthComponent;
	StatsComponent mStats;
	bool isAlive = true;
	int mPlayerIndex;
	int mShootCooldown = 1;
	float mCurrentShootCooldown = 0;
	bool canShoot = false;

};