#pragma once
#include "Boss.h"

class Makhina_Boss : public Boss
{
	public:
	Makhina_Boss(Entity _entity) 
	{
		mPlayerIndex = _entity;
		init();
		InitStats();
	}
	~Makhina_Boss();
	virtual void Update() override;
	virtual void init() override;
	virtual void TakeDamage(float damage) override
	{
		mHealthComponent.mHealth -= damage;
		if (mHealthComponent.mHealth <= 0)
		{
			isAlive = false;
		}
	}
	virtual void InitStats() override
	{
		mStats.SetStats(3000, 0, 0, 0, 30, 10, 0, 60, 0);
		mHealthComponent.mHealth = mStats.mHealth;
	}
	virtual void Move(float _deltaTime) override
	{
		// Boss movement logic here
	}
	virtual void Attack(float _deltaTime) override;
	virtual void LookPlayer() override
	{
		transformSystem::LookAt(mHeadTransform, ECS::GetInstance().getComponent<transformComponent>(mPlayerIndex).position);
	}
	virtual void UpdateComponent() override
	{
		mHeadTransform.position = mTransform.position;
		mHeadTransform.position.y = 2;
		ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
		ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
		if (mHealthComponent.mHealth >  mStats.mHealth) {
			mHealthComponent.mHealth = mStats.mHealth;
		}
		ECS::GetInstance().getComponent<HealthComponent>(mEntity) = mHealthComponent;
		ECS::GetInstance().getComponent<transformComponent>(mHeadEntity) = mHeadTransform;
	}

	virtual void ChangeShootPattern() override;
};