#include "pch.h"
#include "Makhina_Boss.h"

Makhina_Boss::~Makhina_Boss()
{
}

void Makhina_Boss::Update()
{
	UpdateComponent();
}

void Makhina_Boss::init()
{
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 0, 0, 5, 5, 5), ColliderComponent(), HealthComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mHealthComponent = ECS::GetInstance().getComponent<HealthComponent>(mEntity);
	mCollider.depth = mTransform.scale.z;
	mCollider.width = mTransform.scale.x;
	mCollider.height = mTransform.scale.y;
	mCollider.compOwner = mEntity;
	mCollider.updateCollider();
	mStats.SetStats(300, 0, 0, 0, 30, 10, 0, 60, 0);
	mHealthComponent.mHealth = mStats.mHealth;
}
