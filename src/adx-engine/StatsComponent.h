#pragma once

struct StatsComponent
{
	StatsComponent(
		float hp = 100.0f, 
		float strength = 10.0f, 
		float defense = 0.0f, 
		float moveSpeed = 35.0f, 
		float exp = 0.0f, 
		float healthRegenCooldown = 5.0f,
		float manaPoints = 50.0f, 
		float manaRegenCooldown = 5.0f, 
		float magicPower = 10.0f)	
		:
		mMaxHealth(hp),
		mCurrentHealth(hp),
		mStrength(strength), 
		mDefense(defense), 
		mMoveSpeed(moveSpeed), 
		mExp(exp), 
		mHealthRegenCooldown(healthRegenCooldown),
		mManaPoints(manaPoints), 
		mMaxManaPoints(manaPoints), 
		mManaRegenCooldown(manaRegenCooldown), 
		mMagicPower(magicPower){}

	float mCurrentHealth;
	float mMaxHealth;
	float mHealthRegenCooldown;
	float mExp;
	float mStrength;
	float mDefense;
	float mMoveSpeed;
	float mManaPoints;
	float mMaxManaPoints;
	float mManaRegenCooldown;
	float mMagicPower;
	float mCurrentHealthRegenCooldown = 0.0f;
	float mCurrentManaRegenCooldown = 0.0f;
	float mLevel = 1.0f;
};