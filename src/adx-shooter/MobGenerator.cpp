#include "pch.h"
#include "MobGenerator.h"
#include "GameManager.h"




Enemy* MobGenerator::SpawnMob(float x, float z, int mob)
{
	Window* mWindow = GameManager::GetInstance().GetWindow();

	EnemyMarksman* newEnemy = new EnemyMarksman();
	newEnemy->Init(Player::GetInstance().mEntity);
	newEnemy->GetTransform() = ECS::GetInstance().getComponent<transformComponent>(newEnemy->mEntity);
	newEnemy->GetTransform().position = FLOAT3(x, 2, z);

	MeshGeometry newMesh;
	newMesh = MeshCreator::CreateBox(GameManager::GetInstance().GetWindow(), 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

	GameManager::GetInstance().GetWindow()->RegisterExistingMeshForEntity(newEnemy->mEntity);
	GameManager::GetInstance().mEntityMesh.insert({ newEnemy->mEntity, GameManager::GetInstance().gEnemyMesh()});
	XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newEnemy->mEntity));
	GameManager::GetInstance().GetWindow()->Update(newEnemy->mEntity, enemyWorld);
	GameManager::GetInstance().mEnemyList.push_back(newEnemy);
	return newEnemy;
}

Boss* MobGenerator::SpawnBoss(float x, float z)
{
	Window* mWindow = GameManager::GetInstance().GetWindow();

	Boss* newBoss = new Makhina_Boss(Player::GetInstance().mEntity);

	MeshGeometry MakhinaBossMesh;


	newBoss->GetTransform() = ECS::GetInstance().getComponent<transformComponent>(newBoss->GetEntity());
	newBoss->GetTransform().position = FLOAT3(x, newBoss->GetTransform().scale.y / 2, z);
	mWindow->RegisterExistingMeshForEntity(newBoss->GetEntity());
	XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newBoss->GetEntity()));
	GameManager::GetInstance().mEntityMesh.insert({ newBoss->GetEntity(), GameManager::GetInstance().gBossMesh()});
	mWindow->Update(newBoss->GetEntity(), enemyWorld);
	GameManager::GetInstance().mBossList.push_back(newBoss);
	return newBoss;
}