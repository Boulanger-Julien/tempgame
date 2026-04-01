#pragma once
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "Door.h"
#include "MeshCreator.h" 
#include "Enemy\Enemy.h"
#include "adx-render/TextRenderer.h"

class GameManager;

    enum RoomType
    {
        ENEMY_ROOM,
        BOSS_ROOM,
        TREASURE_ROOM,
		LOBBY_ROOM
	};
class Rooms
{
public:


	RoomType type = LOBBY_ROOM;

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

    int EnemyRateRoom = 80;
    int TreasureRateRoom = 20;

	int RoomToBoss = 5;

    std::vector<Enemy*> EnemyRooms;
	int numberOfRoom = 0;

    int numMaxInDunjeon = 0;
    TextRenderer* mNumberOfRoomRenderer;
    Player* mPlayer;
protected: 
    void UpdateComponent();
	Window* mWindow;



};

