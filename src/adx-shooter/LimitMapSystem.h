#pragma once
#include "adx-engine/transform.h"
#include "adx-engine/ColliderComponent.h"
#include "Rooms.h"
struct LimitMapSystem
{
	static void CheckLimitMap(Player& _transform, Rooms& _currentRoom);
};

