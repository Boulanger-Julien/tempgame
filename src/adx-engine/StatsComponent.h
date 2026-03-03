#pragma once

struct StatsComponent
{
	StatsComponent(float hp = 100.0f, float strength = 10.0f, float defense = 5.0f) :
		healthPoints(hp), strength(strength), defense(defense) {}
	float healthPoints;
	float strength;
	float defense;
};