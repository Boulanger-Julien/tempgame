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

	//Window* mWindow;  

    Entity ground;
    ColliderComponent mCollider;
    transformComponent mTransform;
  /*  Entity wall1Entity;
    Entity wall2Entity;*/
    MeshGeometry road;
    //MeshGeometry wall1;
    //MeshGeometry wall2;
	bool generated = false;
	
    std::vector<Enemy*> enemyList;



    int EnemyRateRoom = 45;
    int BossRateRoom = 20;
    int TreasureRateRoom = 35;

protected: 
    void UpdateComponent();
	Window* mWindow;

    Player* mPlayer;
    TextRenderer* mNumberOfRoomRenderer;
	int numberOfRoom = 1;




};

