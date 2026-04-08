#include "pch.h"
#include "EnemyMarksman.h"
#include "adx-core/Timer.h"
#include "State.h"

void EnemyMarksman::OnInit()
{
	distFollowPlayer = 60;
	distAttackPlayer = 30;
	mShootCooldown = 2;

	InitStats(200, 0, 0, 0, 7, 0, 25, 0, 0);
}

void EnemyMarksman::OnUpdate(float _deltaTime)
{
	switch (currentEnemyState)
	{
	case Enemy::NONE:
		break;
	case Enemy::FOLLOWPLAYER:
		FollowState::OnExecute(*this);
		break;
	case Enemy::ATTACKPLAYER:
		AttackState::OnExecute(*this);
		break;
	case Enemy::STUNTED:
		StuntedState::OnExecute(*this);
		break;
	default:
		break;
	}



	if (CheckDistanceToFollowPlayer()) {
		LookPlayer();

		if (CheckDistanceToAttackPlayer()) {
			currentEnemyState = EnemyState::ATTACKPLAYER;
		}
		else
		{
			currentEnemyState = EnemyState::FOLLOWPLAYER;
		}
	}
}

bool EnemyMarksman::Test() {
	return true;
}