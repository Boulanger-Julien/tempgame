#pragma once
#include "adx-engine/transform.h"
#include "adx-engine/ColliderComponent.h"
#include "Rooms.h"

struct RoomGenerator
{
    static void GenerateRoom(Rooms& _room);
	//static void ClearRoom(Rooms& _room);
	static void AddEnemies();
	static void PlaceDoor(Rooms& _room);
	static void PlacePlayer(Rooms& _room);


};

