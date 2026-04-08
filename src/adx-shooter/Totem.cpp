#include "pch.h"
#include "Totem.h"
#include "Rooms.h"
#include "GameManager.h"

Totem::Totem()
{
	mEntityfeur = ECS::GetInstance().createEntity(transformComponent(0, 5, 0), ColliderComponent());
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(mEntityfeur);
	mTransform = ECS::GetInstance().getComponent<transformComponent>(mEntityfeur);
	mTransform.scale = FLOAT3(2, 10, 2);// FLOAT3(2, 10, 2);
	mCollider.depth = mTransform.scale.z;
	mCollider.width = mTransform.scale.x;
	mCollider.height = mTransform.scale.y;
	mCollider.compOwner = mEntityfeur;
	//mCollider.isTrigger = true;
	mCollider.compOwner = mEntityfeur;
	mCollider.updateCollider();


	//mTeleport = ECS::GetInstance().getComponent<TeleportComponent>(m_entity);
}

Totem::~Totem()
{
}
void Totem::Initialize(Window* _window)
{
	TotemMesh = MeshCreator::CreateBox(_window, mEntityfeur, 1.0f, 1, 1, (XMFLOAT4)Colors::Firebrick);
	GameManager::GetInstance().mEntityMesh.insert({ mEntityfeur, TotemMesh });

}

void Totem::Update(RoomType state)
{

	if (state == RoomType::LOBBY_ROOM)
	{
		mTransform.position.z = 0;
	}
	else mTransform.position.z = -3000;

	if (mCollider.collisionCheck(mPlayer))
	{
		int a = 9;
	}

	mCollider.updateCollider();
	UpdateComponent();

}

void Totem::UpdateComponent()
{
	ECS::GetInstance().getComponent<transformComponent>(mEntityfeur) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(mEntityfeur) = mCollider;
}
