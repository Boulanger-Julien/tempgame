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
		mBaseDamage = 10.0f;
	}
	~Basic_Sword() {}
	void Attack() override {
		std::cout << "Basic Sword Attack!" << std::endl;
	}
	void Update() override {
		std::cout << "Basic Sword Update!" << std::endl;
	}
};
