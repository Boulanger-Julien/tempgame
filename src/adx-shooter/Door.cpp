#include "pch.h"
#include "Door.h"


Door::Door()
{
	m_entity = ECS::GetInstance().createEntity(transformComponent(0, 2, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(m_entity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(m_entity);
	//mTeleport = ECS::GetInstance().getComponent<TeleportComponent>(m_entity);
}

Door::~Door()
{
}

void Door::Update()
{

}
