#include "pch.h"
#include "State.h"

void FollowState::OnEnter(Enemy& cur)
{
}

void FollowState::OnExecute(Enemy& cur)
{
	cur.LookPlayer();
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
}	
	
void AttackState::OnExit(Enemy& cur)
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
