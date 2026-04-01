#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Door.h"
#include "MeshCreator.h" 
#include "Enemy\Enemy.h"
#include "adx-render/TextRenderer.h"

class GameManager;

class Rooms
{
public:
    Rooms();   
    ~Rooms();
     void Initialize(Window* _window);
	 void Update();
	 void OnUpdate(float deltatime);

	std::vector<MeshGeometry> roomMeshes;// à voir plus tard
	std::vector<MeshGeometry> wallMeshes;// à voir plus tard
	std::vector<Entity> wallEntities;// à voir plus tard
	//std::vector<Door> doors;// à voir plus tard
    ECS ecs;  
    Door door;

    Entity ground;
    ColliderComponent mCollider;
    transformComponent mTransform;

    MeshGeometry road;
	bool generated = false;

    int EnemyRateRoom = 70;
    int TreasureRateRoom = 15;

	int RoomToBoss = 5;

    std::vector<Enemy*> EnemyRooms;
	int numberOfRoom = -1;

    int numMaxInDunjeon = -2;
protected: 
    void UpdateComponent();
	Window* mWindow;

    Player* mPlayer;
    TextRenderer* mNumberOfRoomRenderer;


};

