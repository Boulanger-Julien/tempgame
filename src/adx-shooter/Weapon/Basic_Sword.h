#pragma once
#include "adx-engine/framework.h"
#include "Sword.h"

//Subclass of Sword that inherits from Weapon
class Basic_Sword : public Sword
{
	public:
	Basic_Sword() {
		mAttackSpeed = 1.0f;
		mRange = 1.0f;
		mRarity = 1.0f;
		mBaseDamage = 5.0f;
		mName = L"Basic_Sword";
	}
	~Basic_Sword() {}
	void Attack() override {
		transformComponent& Trans = ECS::GetInstance().getComponent<transformComponent>(m_entity);
		Trans.rotation.y += 0.5f;
	}
	void Update() override {
		std::cout << "Basic Sword Update!" << std::endl;
	}
};
