#include "pch.h"
#include "State.h"

void NoneState::OnEnter(Enemy& cur)
{
}

void NoneState::OnExecute(Enemy& cur)
{
	if (cur.CheckDistanceToFollowPlayer())
		cur.currentEnemyState = Enemy::FOLLOWPLAYER;
}

void NoneState::OnExit(Enemy& cur)
{
}

void FollowState::OnEnter(Enemy& cur)
{
}

void FollowState::OnExecute(Enemy& cur)
{
	cur.LookPlayer();
	cur.MoveTowardPlayer(Timer::GetDeltatime()); 
	if (cur.CheckDistanceToAttackPlayer()) {
		cur.currentEnemyState = Enemy::ATTACKPLAYER;
	}
}

void FollowState::OnExit(Enemy& cur)
{
}


void AttackState::OnEnter(Enemy& cur)
{	 
}	 
	 
void AttackState::OnExecute(Enemy& cur)
{	
	cur.LookPlayer();
	cur.Attack(Timer::GetInstance()->GetDeltatime());
	if (!cur.CheckDistanceToAttackPlayer()) {
		cur.currentEnemyState = Enemy::NONE;
	}
}	
	
void AttackState::OnExit(Enemy& cur)
{
}

void DyingState::OnEnter(Enemy& cur)
{	 
}	 
	 
void DyingState::OnExecute(Enemy& cur)
{	
	cur.currentDieCooldown += Timer::GetInstance()->GetDeltatime();
	cur.mCollider.isTrigger = true;
	transformSystem::Rotate(cur.mTransform, 0, 0, 100 * Timer::GetInstance()->GetDeltatime());
	if (cur.currentDieCooldown >= cur.timeToDie) {
		cur.isDead = true;
	}
}	
	
void DyingState::OnExit(Enemy& cur)
{
}

void StuntedState::OnEnter(Enemy& cur)
{	 
}	 
	 
void StuntedState::OnExecute(Enemy& cur)
{	
}	
	
void StuntedState::OnExit(Enemy& cur)
{
}
