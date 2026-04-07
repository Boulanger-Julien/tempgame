#pragma once
#include "adx-core/Timer.h"
#include "Enemy/Enemy.h"

// State des ennemies
struct FollowState
{
	static void OnEnter(Enemy& cur);
	static void OnExecute(Enemy& cur);
	static void OnExit(Enemy& cur);
};
struct AttackState
{
	static void OnEnter(Enemy& cur);
	static void OnExecute(Enemy& cur);
	static void OnExit(Enemy& cur);
};
struct StuntedState
{
	static void OnEnter(Enemy& cur);
	static void OnExecute(Enemy& cur);
	static void OnExit(Enemy& cur);
};

