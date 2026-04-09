#include "pch.h"
#include "Door.h"
#include "GameManager.h"

Door::Door() 
{
	mEntity = ECS::GetInstance().createEntity(transformComponent(0, 1, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntity);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntity);
	mTransform.scale = FLOAT3(10, 5, 10);
	mTransform.position.z = 48;
	mCollider.compOwner = mEntity;

	mCollider.updateCollider();

	//mTeleport = ECS::GetInstance().getComponent<TeleportComponent>(m_entity);
}

Door::~Door()
{
}
void Door::Initialize(Window* _window) 
{
	doorMesh = MeshCreator::CreateBox(_window, mEntity, 1.0f, 1, 1, (XMFLOAT4)Colors::Aqua);
	GameManager::GetInstance().mEntityMesh.insert({ mEntity, doorMesh });

}

void Door::Update(int _enemyleft) 
{

	if (_enemyleft <= 0) // Utilisation de VK_LBUTTON pour plus de fiabilit�
	{
		if (mCollider.collisionCheck(mPlayer))
		{
			changeRoom = true;
		}
		else changeRoom = false;
	}
	else
	{
		changeRoom = false;
	}
	UpdateComponent();
}

void Door::UpdateComponent()
{
	ECS::GetInstance().getComponent<transformComponent>(mEntity) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntity) = mCollider;
}
