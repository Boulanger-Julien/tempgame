#include "pch.h"

ColliderComponent::ColliderComponent(int ownerEntity, float w, float h, float d) {
	compOwner = ownerEntity;

	width = w;
	height = h;
	depth = d;

	updateCollider();
}

bool ColliderComponent::collisionCheck(int entity) {
	if (ECS::GetInstance().hasComponent<ColliderComponent>(entity) && ECS::GetInstance().hasComponent<transformComponent>(entity)) {
		updateCollider();
		return aabb.Contains(ECS::GetInstance().getComponent<transformComponent>(entity).position);
	}
	return 0;
}

void ColliderComponent::updateCollider() {
	aabb.min = ECS::GetInstance().getComponent<transformComponent>(compOwner).position;
	aabb.max.x = aabb.min.x + width;
	aabb.max.y = aabb.min.y + height;
	aabb.max.z = aabb.min.z + depth;
}