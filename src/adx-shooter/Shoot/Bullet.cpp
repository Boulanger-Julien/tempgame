#include "pch.h"
#include "Bullet.h"
#include "adx-core/Timer.h"

Bullet::Bullet() {
	mEntity = ECS::GetInstance().createEntity(transformComponent(), ColliderComponent());
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).depth = mTransform.scale.z;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).width = mTransform.scale.x;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).height = mTransform.scale.y;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).compOwner = mEntity;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).updateCollider();

}

Bullet::~Bullet() {
	ECS::GetInstance().Release(mEntity);
}

void Bullet::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();

	if (currentLifetime >= maxLifetime) {
		toBeDestroyed = true;
		return;
	}
	currentLifetime += deltaTime;

	transformSystem::MoveForward(ECS::GetInstance().getComponent<transformComponent>(mEntity), m_speed*deltaTime);
	UpdateComponent();
}

void Bullet::UpdateComponent() {
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity).updateCollider();
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
}