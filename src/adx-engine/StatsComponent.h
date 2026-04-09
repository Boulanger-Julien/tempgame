#pragma once

struct StatsComponent
{
	void SetStats(
		float _health,
		float _healthRegen,
		float _strength,
		float _defense,
		float _moveSpeed,
		float _exp)	
	{
		mHealth = _health;
		mHealthRegen = _healthRegen;

		mStrength = _strength;

		mDefense = _defense;
		mSpeed = _moveSpeed;
		mExp = _exp;
	}

	float mHealth;
	float mHealthRegen;

	float mSpeed;

	float mStrength;

	float mDefense;
	float mExp;
	float mLevel = 1.0f;
	float mExpToNextLevel = 125.0f;
	float mLevelUpMultiplier = 1.3f;
	int mStatsPointsToAllocate = 0;
};