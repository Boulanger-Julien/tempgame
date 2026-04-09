#pragma once
#include "adx-core/adx_aabb.h"	
#include <DirectXCollision.h>
#include <DirectXMath.h>


struct ColliderComponent
{
	int compOwner;

	float width;
	float height;
	float depth;
	adx_aabb aabb;

	bool isTrigger = true;
	DirectX::BoundingOrientedBox orientedBox;
	ColliderComponent(int ownerEntity = 1, float w = 1.0f, float h = 1.0f, float d = 1.0f);
	void updateCollider();
	void updateOtherCollider(int entity);

	// TODO - move that elsewhere
	bool collisionCheck(int entity);
};