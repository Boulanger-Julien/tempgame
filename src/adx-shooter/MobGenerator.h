#pragma once
#include "adx-engine/transform.h"
#include "adx-engine/ColliderComponent.h"
#include "Enemy/EnemyMarksman.h"
#include "Enemy/Makhina_Boss.h"
#include "Enemy/Boss.h"

struct MobGenerator
{
	static Enemy* SpawnMob(float x, float z, float hp);
	
	static Boss* SpawnBoss(float x, float z);

};
