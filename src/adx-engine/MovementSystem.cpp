#include "pch.h"

void MovementSystem::Update(float deltaTime, ECS ecs)
{
	Move(deltaTime, ecs);
}

void MovementSystem::Move(float deltaTime, ECS ecs)
{
	for (int entity = 0; entity < ecs.getEntityCount(); ++entity) {
		if (ecs.hasComponent<transformComponent>(entity) && ecs.hasComponent<velocityComponent>(entity)) {
			auto& transform = ecs.getComponent<transformComponent>(entity);
			auto& velocity = ecs.getComponent<velocityComponent>(entity);
			FLOAT3 velo(velocity.velocity);
			transform.position.x += velo.x * deltaTime;
			transform.position.y += velo.y * deltaTime;
			transform.position.z += velo.z * deltaTime;
		}
	}

}
