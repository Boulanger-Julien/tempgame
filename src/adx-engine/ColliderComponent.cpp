#include "pch.h"

ColliderComponent::ColliderComponent(int ownerEntity, float w, float h, float d) {
	compOwner = ownerEntity;

	width = w;
	height = h;
	depth = d;

	isTrigger = false;
}

bool ColliderComponent::collisionCheck(int entity) {
	if (ECS::GetInstance().hasComponent<ColliderComponent>(entity)) {
		auto& otherCollider = ECS::GetInstance().getComponent<ColliderComponent>(entity);

		this->updateCollider();
		otherCollider.updateOtherCollider(entity);

		return this->aabb.Intersects(otherCollider.aabb);
	}	return false;
}

void ColliderComponent::updateCollider() {
    auto& transform = ECS::GetInstance().getComponent<transformComponent>(compOwner);

    float halfW = (width * transform.scale.x) / 2.0f;
    float halfH = (height * transform.scale.y) / 2.0f;
    float halfD = (depth * transform.scale.z) / 2.0f;

	// 2. On définit les bornes autour de la position centrale en fonction de la taille et de l'échelle et de la rotation
    //aabb.min.x = transform.position.x - halfW;
    //aabb.min.y = transform.position.y - halfH;
    //aabb.min.z = transform.position.z - halfD;

    //aabb.max.x = transform.position.x + halfW;
    //aabb.max.y = transform.position.y + halfH;
    //aabb.max.z = transform.position.z + halfD;

	aabb.min.x = transform.forward.x * halfD + transform.right.x * halfW + transform.up.x * halfH + transform.position.x;
	aabb.min.y = transform.forward.y * halfD + transform.right.y * halfW + transform.up.y * halfH + transform.position.y;
	aabb.min.z = transform.forward.z * halfD + transform.right.z * halfW + transform.up.z * halfH + transform.position.z;

	aabb.max.x = -transform.forward.x * halfD - transform.right.x * halfW - transform.up.x * halfH + transform.position.x;
	aabb.max.y = -transform.forward.y * halfD - transform.right.y * halfW - transform.up.y * halfH + transform.position.y;
	aabb.max.z = -transform.forward.z * halfD - transform.right.z * halfW - transform.up.z * halfH + transform.position.z;
}

void ColliderComponent::updateOtherCollider(int entity) {
    auto& transform = ECS::GetInstance().getComponent<transformComponent>(entity);

    float halfW = (width * transform.scale.x) / 2.0f;
    float halfH = (height * transform.scale.y) / 2.0f;
    float halfD = (depth * transform.scale.z) / 2.0f;

    // 2. On définit les bornes autour de la position centrale
    aabb.min.x = transform.position.x - halfW;
    aabb.min.y = transform.position.y - halfH;
    aabb.min.z = transform.position.z - halfD;

    aabb.max.x = transform.position.x + halfW;
    aabb.max.y = transform.position.y + halfH;
    aabb.max.z = transform.position.z + halfD;
}

