#pragma once
#include "adx-core/adx_aabb.h"	

struct ColliderComponent
{
	int compOwner;

	float width;
	float height;
	float depth;
	adx_aabb aabb;

	bool isTrigger = false;

	ColliderComponent(int ownerEntity, float w = 1.0f, float h = 1.0f, float d = 1.0f);
	void updateCollider();

	// TODO - move that elsewhere
	bool collisionCheck(int entity);
};