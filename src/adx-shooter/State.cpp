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
	float dt = Timer::GetInstance()->GetDeltatime();
	//cur.currentDieCooldown += Timer::GetInstance()->GetDeltatime();
	cur.mCollider.isTrigger = false;
	transformSystem::Rotate(cur.mTransform, 0, 500 * dt, 0);
	cur.mTransform.scale = FLOAT3(cur.mTransform.scale.x - dt, cur.mTransform.scale.y - dt, cur.mTransform.scale.z - dt);
	if (cur.mTransform.scale.x <=0) {
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

void DyingBossState::OnEnter(Boss& cur)
{	 
}	 
	 
void DyingBossState::OnExecute(Boss& cur)
{	
	float dt = Timer::GetInstance()->GetDeltatime();
	cur.mCollider.isTrigger = false;


	if (cur.mTransform.position.y < 10)
	{
		//transformSystem::Rotate(cur.mTransform, 0, cur.mSpeedDying, 0);
		cur.mTransform.scale = FLOAT3(cur.mTransform.scale.x +sin(cur.deathTimer) *20*dt, cur.mTransform.scale.y + sin(cur.deathTimer) *20* dt, cur.mTransform.scale.z + sin(cur.deathTimer) *20* dt);
		transformSystem::Rotate(cur.mTransform , 0 , cur.mSpeedDying *dt, 0);
		cur.mTransform.scale = FLOAT3(cur.mTransform.scale.x + dt , cur.mTransform.scale.y + dt , cur.mTransform.scale.z + dt);
		cur.mTransform.position.y += 2 * dt;
		cur.deathTimer += 40*dt;
	}
	else
	{
	cur.mTransform.scale = FLOAT3(cur.mTransform.scale.x - dt *5, cur.mTransform.scale.y - dt * 5 , cur.mTransform.scale.z - dt * 5);
	transformSystem::Rotate(cur.mTransform, 0, cur.mSpeedDying * dt, 0);
	cur.mSpeedDying += 100 * dt;
	}
	

	if (cur.mTransform.scale.x <= 0) {
		cur.isAlive = false;
	}
}	
	
void DyingBossState::OnExit(Boss& cur)
{
}

void CommingBackBossState::OnEnter(Boss& cur)
{	 
}	 
	 
void CommingBackBossState::OnExecute(Boss& cur)
{	
	float dt = Timer::GetInstance()->GetDeltatime();

		if (cur.mTransform.position.z > 0)
		{
			cur.mTransform.position.z -= 20* dt;
		}
		else cur.mTransform.position.z = 0;

		if (cur.mTransform.position.y > 5)
		{
			cur.mTransform.position.y -= 20* dt;
		}
		else cur.mTransform.position.y = 5;
		transformSystem::Rotate(cur.mTransform, -cur.SpeedEntering * dt,0,0);
		cur.SpeedEntering += 20 * dt;
}	
	
void CommingBackBossState::OnExit(Boss& cur)
{
}
