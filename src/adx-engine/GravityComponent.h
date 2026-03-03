#pragma once 

struct GravityComponent
{
	GravityComponent(float g = 9.81f) : gravity(g) {}
	float gravity;
	bool isAffectedByGravity = true;
};