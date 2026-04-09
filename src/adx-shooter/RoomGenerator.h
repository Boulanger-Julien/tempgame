#pragma once
#include "adx-engine/transform.h"
#include "adx-engine/ColliderComponent.h"
#include "Rooms.h"

struct RoomGenerator
{
    static void GenerateRoom(Rooms& _room);
	static void AddEnemies(Rooms& _room, float hppercentage = 0);
	static void PlaceDoor(Rooms& _room);
	static void PlacePlayer(Rooms& _room);
};

