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
        auto& other = ECS::GetInstance().getComponent<ColliderComponent>(entity);
		other.compOwner = entity;

        // On met à jour les deux boîtes avant le test
        this->updateCollider();
		other.updateCollider();
        // Si l'autre est une simple AABB (comme un ennemi de base), 
        // DirectX gère l'intersection OBB vs AABB automatiquement
        DirectX::BoundingBox otherAABB;

        return this->orientedBox.Intersects(other.orientedBox); // Test OBB vs OBB
    }
    return false;
}

void ColliderComponent::updateCollider() {
    auto& transform = ECS::GetInstance().getComponent<transformComponent>(compOwner);

    // 1. Position centrale
    orientedBox.Center = { transform.position.x, transform.position.y, transform.position.z };

    // 2. Demi-dimensions (Extents)
    orientedBox.Extents = {
        (width * transform.scale.x) / 2.0f,
        (height * transform.scale.y) / 2.0f,
        (depth * transform.scale.z) / 2.0f
    };

    // 3. Rotation (Quaternion)
    // DirectX Collision utilise des Quaternions pour l'orientation
    // Si ton transform a des angles d'Euler (rotation.x, y, z) :
    DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(
        transform.rotation.x,
        transform.rotation.y,
        transform.rotation.z
    );
    XMStoreFloat4(&orientedBox.Orientation, q);
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

