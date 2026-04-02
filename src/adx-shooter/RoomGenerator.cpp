#include "pch.h"
#include "RoomGenerator.h"
#include "GameManager.h"

#define NEXT_ENEMY 1
#define NEXT_BOSS 2
#define NEXT_TREASURE 3
#define NEXT_LOBBY 4
void RoomGenerator::GenerateRoom(Rooms& _room)
{
	int choiceRoom = NULL;
    switch (_room.type)
    {
    case RoomType::ENEMY_ROOM:
        _room.numberOfRoom++;
        _room.RoomToBoss--;
        break;
    case RoomType::BOSS_ROOM:
        _room.numberOfRoom++;
        _room.RoomToBoss = 5;
		choiceRoom = NEXT_TREASURE;
        break;
    case RoomType::TREASURE_ROOM:
        _room.numberOfRoom++;
        _room.RoomToBoss--;
        break;
    case RoomType::LOBBY_ROOM:
        _room.numberOfRoom = 1;
		_room.numMaxInDunjeon = rand() % 10 + 6;
        _room.RoomToBoss --;
        break;
	default:
		break;
    };

    if (_room.mPlayer->GetHealthComponent().mHealth <= 0)
    {
        choiceRoom = NEXT_LOBBY;
        _room.numberOfRoom = 0;
        _room.numMaxInDunjeon = 0;
        _room.RoomToBoss = 5;
	}

    if (choiceRoom == NULL)
    {
        int color = (rand() % _room.EnemyRateRoom + _room.TreasureRateRoom) + 1;
        if (color <= _room.EnemyRateRoom) {
            choiceRoom = NEXT_ENEMY;
        }
        else if (color <= _room.EnemyRateRoom + _room.TreasureRateRoom) {
            choiceRoom = NEXT_TREASURE;
        }
        if (_room.RoomToBoss == 0)
        {
            choiceRoom = NEXT_BOSS;
            _room.RoomToBoss = 5;
        }
        if (_room.numberOfRoom > _room.numMaxInDunjeon)
        {
            choiceRoom = NEXT_LOBBY;
            _room.numberOfRoom = 0;
            _room.numMaxInDunjeon = 0;
            _room.RoomToBoss = 5;
        }
    }

     switch (choiceRoom)
     {
        case NEXT_ENEMY:
			_room.type = RoomType::ENEMY_ROOM;
            AddEnemies(_room);
			break;
        case NEXT_BOSS:
            _room.type = RoomType::BOSS_ROOM;
            _room.BossList.push_back( GameManager::GetInstance().SpawnBoss(0,0));
            break;
		case NEXT_TREASURE:
            _room.type = RoomType::TREASURE_ROOM;
            break;
		case NEXT_LOBBY:
            _room.type = RoomType::LOBBY_ROOM;
            break;
		default:
			break;
     };

     PlacePlayer(_room);
     PlaceDoor(_room);
}


void RoomGenerator::AddEnemies(Rooms& _room)
{
    int numberOfEnnemies = (rand() % 13) + 2;
    for (int i = 0; i < 10; i++) {
        _room.EnemyRooms.push_back(GameManager::GetInstance().SpawnMob(rand() % 100 - 48, rand() % 100 - 48, 0));
    }
}

void RoomGenerator::PlaceDoor(Rooms& _room)
{
    if (_room.type == RoomType::LOBBY_ROOM || _room.type == RoomType::BOSS_ROOM) {
        _room.door.mTransform.position = FLOAT3(0, 1, 48); // Porte en haut
        return;
    }
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
    if (_room.type == RoomType::LOBBY_ROOM) {
        _room.mPlayer->GetTransform().position = FLOAT3(0, 2, 0);
        return;
	}
    FLOAT3 posDoor = _room.door.mTransform.position;
    GameManager::GetInstance().GetPlayer()->GetTransform().position = FLOAT3(-posDoor.x, 2, -posDoor.z);
}
