#include "pch.h"
#include "RoomGenerator.h"
#include "GameManager.h"

#define ENEMY_ROOM 1
#define BOSS_ROOM 2
#define TREASURE_ROOM 3
void RoomGenerator::GenerateRoom(Rooms& _room)
{
        //mWindow->RemoveEntityResources(currentRoom.ground);
        int color = (rand() % 100) + 1;
        int choiceRoom= NULL;
     if (color <= _room.EnemyRateRoom) {
         choiceRoom = ENEMY_ROOM;
	 }
     else if (color <= _room.EnemyRateRoom + _room.BossRateRoom) {
         choiceRoom = BOSS_ROOM;
     }
     else if (color <= _room.EnemyRateRoom + _room.BossRateRoom + _room.TreasureRateRoom) {
         choiceRoom = TREASURE_ROOM; // TREASURE ROOM
     }

     switch (choiceRoom)
     {
        case ENEMY_ROOM:
            AddEnemies();
			break;
        case BOSS_ROOM:
            GameManager::GetInstance().SpawnBoss(0,0);
            break;
		case TREASURE_ROOM:
            break;
		default:
			break;
     };

     PlacePlayer(_room);
     PlaceDoor(_room);
}


void RoomGenerator::AddEnemies()
{
    int numberOfEnnemies = (rand() % 13) + 2;
    for (int i = 0; i < 10; i++) {
        GameManager::GetInstance().SpawnMob(rand() % 100 - 48, rand() % 100 - 48, 0);
    }
}

void RoomGenerator::PlaceDoor(Rooms& _room)
{
    int direction = (rand() % 2) + 1; 
	int linePosition = (rand() % 80) - 40;
    if (direction == 1) {
        _room.door.mTransform.position = FLOAT3(linePosition, 1, 48); // Porte en haut
    }
    else {
        _room.door.mTransform.position = FLOAT3(-48, 1, linePosition); // Porte en bas
	}
}

void RoomGenerator::PlacePlayer(Rooms& _room)
{
    FLOAT3 posDoor = _room.door.mTransform.position;
  GameManager::GetInstance().GetPlayer()->GetTransform().position = FLOAT3(-posDoor.x, 2, -posDoor.z);
}
