#pragma once
#include "adx-engine/framework.h"

class Weapon
{
	public:
	Weapon() {
		m_entity = ECS::GetInstance().createEntity(transformComponent());}
	virtual ~Weapon() {}
	virtual void Attack() = 0;
	virtual void Update() = 0;
	virtual const wchar_t* GetName() { return mName; }
	virtual float GetEntity() { return m_entity; }
protected:
	const wchar_t* mName;
	float Substat;
	float mAttackSpeed;
	float mRange;
	float mRarity;
	float mBaseDamage;
	Entity m_entity;
};
