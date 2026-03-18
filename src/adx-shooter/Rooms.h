#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Door.h"
#include "MeshCreator.h" 
class Rooms
{
public:
    Rooms();   
     ~Rooms() {}
     void Initialize();
	 void Draw();
	std::vector<MeshGeometry> roomMeshes;// à voir plus tard
    //std::vector<Door> doors;
	//std::vector<Door> doors;// à voir plus tard
    ECS ecs;  
    Door door;
	//Window* mWindow;  
    Entity ground;// = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
    MeshGeometry road;// = MeshCreator::CreateBox(mWindow, road1, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
    //mEntityMesh.insert({ road1, road });  
	bool generated = false;
protected: 
};

