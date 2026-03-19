#include "pch.h"
#include "EnemyMarksman.h"
#include "adx-core/Timer.h"

void EnemyMarksman::OnInit()
{
	distFollowPlayer = 60;
	distAttackPlayer = 30;
	mShootCooldown = 2;

	InitStats(200, 0, 0, 0, 7, 0, 25, 0, 0);
}

void EnemyMarksman::OnUpdate(float _deltaTime)
{
	if (CheckDistanceToFollowPlayer()) {
		LookPlayer();

		if (CheckDistanceToAttackPlayer()) {
			Attack(_deltaTime);
		}
		else
		{
			MoveTowardPlayer(_deltaTime);
		}
	}
}

bool EnemyMarksman::Test() {
	return true;
}