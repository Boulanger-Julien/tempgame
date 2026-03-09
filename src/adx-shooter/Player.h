#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"

class Player
{
public:
	StatsComponent Stats;
	Entity m_entity;

	Player();
	void ChooseClass(int classID);
	void Update(const Ray& mouseRay);
	void takeDamage(int damage);
	transformComponent& GetTransform() { return mTransform; }
	ColliderComponent& GetCollider() { return mCollider; }

private:
	transformComponent mTransform;
	ColliderComponent mCollider;
};

