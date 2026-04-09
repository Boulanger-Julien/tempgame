#pragma once
#include "adx-core/Timer.h"
#include "Enemy/Enemy.h"
#include "Enemy/Boss.h"

// State des ennemies
struct NoneState
{
	static void OnEnter(Enemy& cur);
	static void OnExecute(Enemy& cur);
	static void OnExit(Enemy& cur);
};
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
struct DyingState
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

//BossStates
struct DyingBossState
{
	static void OnEnter(Boss& cur);
	static void OnExecute(Boss& cur);
	static void OnExit(Boss& cur);
};

