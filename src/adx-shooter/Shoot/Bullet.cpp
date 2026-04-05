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
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);

}

Bullet::~Bullet() {
	ECS::GetInstance().Release(mEntity);
}

void Bullet::Update() {
	float deltaTime = Timer::GetInstance()->GetDeltatime();

	currentLifetime += deltaTime;

	transformSystem::MoveForward(ECS::GetInstance().getComponent<transformComponent>(mEntity), m_speed*deltaTime);
	UpdateComponent();
}

void Bullet::UpdateComponent() {

	mCollider.updateCollider();
	mTransform.position = mCollider.orientedBox.Center;
	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
}