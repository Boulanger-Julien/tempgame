#pragma once

struct StatsComponent
{
	void SetStats(
		float _health,
		float _healthRegen,
		float _mana,
		float _manaRegen,
		float _strength,
		float _defense,
		float _moveSpeed,
		float _exp,
		float _magicPower)	
	{
		mHealth = _health;
		mHealthRegen = _healthRegen;

		mMana = _mana;
		mManaRegen = _manaRegen;

		mStrength = _strength;

		mDefense = _defense;
		mSpeed = _moveSpeed;
		mExp = _exp;
		mMagicPower = _magicPower;
	}

	float mHealth;
	float mHealthRegen;

	float mMana;
	float mManaRegen;

	float mSpeed;

	float mStrength;

	float mDefense;
	float mExp;
	float mMagicPower;
	float mLevel = 1.0f;
};