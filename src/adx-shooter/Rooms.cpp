#include "pch.h"
#include "Rooms.h"

Rooms::Rooms()
{
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
	//road = MeshCreator::CreateBox(mWindow, ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
	door.mTransform.position = FLOAT3(0, 0, 50); 
	//mEntityMesh.insert({ ground, road });
}


void Rooms::Initialize()
{
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
}