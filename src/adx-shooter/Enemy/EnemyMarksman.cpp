#include "pch.h"
#include "EnemyMarksman.h"
#include "adx-core/Timer.h"
#include "State.h"

void EnemyMarksman::OnInit()
{
	distFollowPlayer = 60;
	distAttackPlayer = 30;
	mShootCooldown = 2;

	InitStats(100, 0, 7, 0, 25, 40);
}

void EnemyMarksman::OnUpdate(float _deltaTime)
{
	switch (currentEnemyState)
	{
	case Enemy::NONE:
		NoneState::OnExecute(*this);
		break;
	case Enemy::FOLLOWPLAYER:
		FollowState::OnExecute(*this);
		break;
	case Enemy::ATTACKPLAYER:
		AttackState::OnExecute(*this);
		break;
	case Enemy::DYING:
		DyingState::OnExecute(*this);
		break;
		//case Enemy::STUNTED:				//n'existe pas encore
	//	StuntedState::OnExecute(*this);
	//	break;
	default:
		break;
	}
}

bool EnemyMarksman::Test() {
	return true;
}