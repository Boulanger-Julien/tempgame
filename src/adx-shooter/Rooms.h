#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Door.h"
#include "MeshCreator.h" 
#include "Enemy.h"
class Rooms
{
public:
    Rooms();   
    ~Rooms();
     void Initialize(Window* _window);
	 void Update();
	 void OnUpdate(float deltatime);

	std::vector<MeshGeometry> roomMeshes;// à voir plus tard
	//std::vector<Door> doors;// à voir plus tard
    ECS ecs;  
    Door door;

	//Window* mWindow;  
    Entity ground;
    Entity wall1Entity;
    Entity wall2Entity;
    MeshGeometry road;
    MeshGeometry wall1;
    MeshGeometry wall2;
	bool generated = false;
	
protected: 
	Window* mWindow;
};

