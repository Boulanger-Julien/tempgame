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
    int BossRateRoom = 15;
    int TreasureRateRoom = 15;

    std::vector<Enemy*> EnemyRooms;
protected: 
    void UpdateComponent();
	Window* mWindow;

    Player* mPlayer;
    TextRenderer* mNumberOfRoomRenderer;
	int numberOfRoom = 1;




};

