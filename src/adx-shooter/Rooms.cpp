#include "pch.h"
#include "Rooms.h"

Rooms::Rooms()
{
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
	wall1Entity = ECS::GetInstance().createEntity(transformComponent(0, 5, 50));
	wall2Entity = ECS::GetInstance().createEntity(transformComponent(-50, 5, 0));
	//road = MeshCreator::CreateBox(mWindow, ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray); 
	//mEntityMesh.insert({ ground, road });
}

Rooms::~Rooms()
{
	mWindow->RemoveEntityResources(door.mEntity);
}


void Rooms::Initialize(Window* _window)
{
	mWindow = _window;
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));

	road = MeshCreator::CreateBox(mWindow, ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
	wall1 = MeshCreator::CreateBox(mWindow, wall1Entity, 100.0f, 10, 1, (XMFLOAT4)Colors::PeachPuff);
	wall2 = MeshCreator::CreateBox(mWindow, wall2Entity, 1, 10, 100, (XMFLOAT4)Colors::Violet);
	
	door.Initialize(mWindow);
}
void Rooms::Update()
{
	float deltatime = Timer::GetInstance()->GetDeltatime();
	
	OnUpdate(deltatime);
}
void Rooms::OnUpdate(float _dt)
{
	if (door.changeRoom) {
		door.changeRoom = false;
		// Générer une nouvelle salle
		//currentRoom = Rooms();
	}
}