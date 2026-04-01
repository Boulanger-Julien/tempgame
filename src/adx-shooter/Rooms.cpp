#include "pch.h"
#include "Rooms.h"
#include "GameManager.h"
#include "RoomGenerator.h"
#include "LimitMapSystem.h"

Rooms::Rooms()
{
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0), ColliderComponent());
	mTransform = ECS::GetInstance().getComponent<transformComponent>(ground);
	mTransform.scale = FLOAT3(100, 1, 100);
	mCollider = ECS::GetInstance().getComponent<ColliderComponent>(ground);
	//road = MeshCreator::CreateBox(mWindow, ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray); 
	//mEntityMesh.insert({ ground, road });

	
}

Rooms::~Rooms()
{
	mWindow->RemoveEntityResources(door.mEntity);
}

void Rooms::Initialize(Window* _window)
{
	mWindow = _window;
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));

	road = MeshCreator::CreateBox(mWindow, ground, 1.0f, 1, 1, (XMFLOAT4)Colors::Gray);

	wallEntities.push_back(ECS::GetInstance().createEntity(transformComponent(0, 5, 50), ColliderComponent()));
	wallEntities.push_back(ECS::GetInstance().createEntity(transformComponent(-50, 5, 0), ColliderComponent()));
	wallMeshes.push_back(MeshCreator::CreateBox(mWindow, wallEntities[0], 100.0f, 10, 1, (XMFLOAT4)Colors::PeachPuff));
	wallMeshes.push_back(MeshCreator::CreateBox(mWindow, wallEntities[1], 1, 10, 100, (XMFLOAT4)Colors::Violet));

	GameManager::GetInstance().mEntityMesh.insert({ ground, road });
	for (int i = 0; i < wallEntities.size(); ++i)
	{
		GameManager::GetInstance().mEntityMesh.insert({ wallEntities[i], wallMeshes[i]});
	}



	mPlayer = GameManager::GetInstance().GetPlayer();
	door.Initialize(mWindow);
	mNumberOfRoomRenderer = new TextRenderer(_window);
	mNumberOfRoomRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
}
void Rooms::Update()
{
	float deltatime = Timer::GetInstance()->GetDeltatime();
	UpdateComponent();

	OnUpdate(deltatime);
	door.Update(EnemyRooms.size());
	for (Enemy* enemy : EnemyRooms)
	{
		if (enemy->isDead)
		{
			auto it = std::find(EnemyRooms.begin(), EnemyRooms.end(), enemy);
			if (it != EnemyRooms.end()) {
				EnemyRooms.erase(it);
			}
		}
	}
}
void Rooms::OnUpdate(float _dt)
{
	ECS::GetInstance().getComponent<ColliderComponent>(ground) = mCollider;
	LimitMapSystem::CheckLimitMap(*mPlayer, *this);

	if (InputSystem::isKeyUp('E') && door.changeRoom == true)
	{

		RoomGenerator::GenerateRoom(*this);
	}
	
}

void Rooms::UpdateComponent()
{
	ECS::GetInstance().getComponent<transformComponent>(ground) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(ground) = mCollider;
}