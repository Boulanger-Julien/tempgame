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
	timerStr = std::format("Timer : {:02}:{:02}:{:02}.{:02}",
		hours, minutes, seconds, centiemes);
	
}

Rooms::~Rooms()
{
	mWindow->RemoveEntityResources(door.mEntity);
	mWindow->RemoveEntityResources(totem.mEntityfeur);
}

void Rooms::Initialize(Window* _window)
{
	mWindow = _window;
	ground = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));

	road = MeshCreator::CreateBox(mWindow, ground, 1.0f, 1, 1, (XMFLOAT4)Colors::Gray);

	wallEntities.push_back(ECS::GetInstance().createEntity(transformComponent(0, 5, 50), ColliderComponent()));
	wallEntities.push_back(ECS::GetInstance().createEntity(transformComponent(-50, 5, 0), ColliderComponent()));
	wallMeshes.push_back(MeshCreator::CreateBox(mWindow, wallEntities[0], 1, 1, 1, (XMFLOAT4)Colors::PeachPuff));
	wallMeshes.push_back(MeshCreator::CreateBox(mWindow, wallEntities[1], 1, 1, 1, (XMFLOAT4)Colors::Violet));
	
	ECS::GetInstance().getComponent<transformComponent>(wallEntities[0]).scale = FLOAT3(100.0f, 10, 1);
	ECS::GetInstance().getComponent<transformComponent>(wallEntities[1]).scale = FLOAT3(1, 10, 100);

	wallColliders.push_back(ECS::GetInstance().getComponent<ColliderComponent>(wallEntities[0]));
	wallColliders.push_back(ECS::GetInstance().getComponent<ColliderComponent>(wallEntities[1]));
	GameManager::GetInstance().mEntityMesh.insert({ ground, road });


	for (int i = 0; i < wallEntities.size(); ++i)
	{
		GameManager::GetInstance().mEntityMesh.insert({ wallEntities[i], wallMeshes[i]});
	}

	mWindow->RegisterExistingMeshForEntity(wallEntities[0]);
	mWindow->RegisterExistingMeshForEntity(wallEntities[1]);


	mPlayer = GameManager::GetInstance().mPlayer;
	door.Initialize(mWindow);
	totem.Initialize(mWindow);
	mNumberOfRoomRenderer = new TextRenderer();

	mTimerTextRenderer = new TextRenderer();

	mChooseDifficultyRenderer = new TextRenderer();

	mPatternTextRenderer = new TextRenderer();

}

void Rooms::Update()
{
	float deltatime = Timer::GetInstance()->GetDeltatime();

	mCollider.updateCollider();
	UpdateComponent();

	OnUpdate(deltatime);
	totem.Update(type);
	door.Update(EnemyRooms.size() + BossList.size());
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

	for (Boss* enemy : BossList)
	{
		if (enemy->IsAlive() == false)
		{
			auto it = std::find(BossList.begin(), BossList.end(), enemy);
			if (it != BossList.end()) {
				BossList.erase(it);
			}
		}
	}

	for (ColliderComponent entity : wallColliders)
	{
		entity.updateCollider();
	}
}


void Rooms::Draw()
{
	mTimerTextRenderer->DrawTxt(timerStr, 20, 60, 24); 
	mNumberOfRoomRenderer->DrawTxt("Room " + std::to_string(numberOfRoom) + " / " + std::to_string(numMaxInDunjeon), 40, 90, 24);
		mChooseDifficultyRenderer->DrawTxt(mDifficultyStr, 20, 220, 24);
	if (isChoosingDifficulty)
	{
		mDifficultyStr = "Choose difficulty : 1 - Easy | 2 - Medium | 3 - Hard";
	}
	else mDifficultyStr = "";

	if (isChoosingPattern)
	{
		if (attackTochange != 0)
		{
			mPatternTextRenderer->DrawTxt("Choose new pattern for attack " + std::to_string(attackTochange) + " : 1 - Single Shot | 2 -  Explosion |\n3 - Pump | 4 - Line | 5 - Choc | 6 - Thunder | 7 - Boomerang | 8 - Bomb"
				, 20, 220, 24);
		}
		else
		{
			mPatternTextRenderer->DrawTxt("Choose attack to change : 1 - First | 2 - Second | 3 - Third", 20, 220, 24);
		}
	}
}


void Rooms::OnUpdate(float _dt)
{
	ECS::GetInstance().getComponent<ColliderComponent>(ground) = mCollider;
	
	if (mPlayer->GetHealthComponent().mHealth <= 0)
	{
		GameManager* gameManager = &GameManager::GetInstance();
		for (Bullet* bullet : GameManager::GetInstance().mBulletList) {
			bullet->toBeDestroyed = true;
		}
		for (Enemy* enemy : EnemyRooms) {
			enemy->isDead = true;
		}
		for (Boss* boss : BossList) {
			boss->TakeDamage(boss->GetHealth());
		}
		//GameManager::GetInstance().Destroy();
		RoomGenerator::GenerateRoom(*this);
		timer = 0.0f;
	}
	else if (type != LOBBY_ROOM)
	{
		timer += _dt;
		seconds = (int)timer % 60;
		minutes = ((int)timer % 3600) / 60;
		hours = (int)timer / 3600;
		centiemes = (int)(timer * 100) % 100;
		timerStr = std::format("Timer : {:02}:{:02}:{:02}.{:02}",
			hours, minutes, seconds, centiemes);
	}
	
	if (InputSystem::isKeyUp('E'))
	{
		if (door.changeRoom)
		{
			if (mdifficulty != 0)
			{
				RoomGenerator::GenerateRoom(*this);
			}
			else
			{
				isChoosingDifficulty = true;
			}
		}
		else if (totem.changePattern)
		{
			isChoosingPattern = true;
		}
	}

	if (isChoosingDifficulty)
	{
		if (InputSystem::isKeyDown('1'))
		{
			mdifficulty = 1;
			isChoosingDifficulty = false;
			RoomGenerator::GenerateRoom(*this);
		}
		else if (InputSystem::isKeyDown('2'))
		{
			mdifficulty = 2;
			isChoosingDifficulty = false;
			RoomGenerator::GenerateRoom(*this);
		}
		else if (InputSystem::isKeyDown('3'))
		{
			mdifficulty = 3;
			isChoosingDifficulty = false;
			RoomGenerator::GenerateRoom(*this);
		}
	}	
	static bool eDownLastFrame = false;
	static int test = 0;
	if (isChoosingPattern)
	{
		if (attackTochange == 0)
		{
			if (InputSystem::isKeyUp('1')) { attackTochange = 1; eDownLastFrame = true; }
			else if (InputSystem::isKeyUp('2')) { attackTochange = 2; eDownLastFrame = true; }
			else if (InputSystem::isKeyUp('3')) { attackTochange = 3; eDownLastFrame = true; }
		}
		else if (!eDownLastFrame)
		{
			char key = InputSystem::GetKeyDown();
			int value = key - '0';

			if (value >= 1 && value <= ShootPatternType::Amount - 1 && !eDownLastFrame)
			{
				mPlayer->ChangePattern(attackTochange, value);
				attackTochange = 0;
				isChoosingPattern = false;
			}

		test++;
		}
		eDownLastFrame = false; // Maintenant il ne se remet à false que si aucune touche n'est active
	}
}

void Rooms::UpdateComponent()
{
	for (ColliderComponent entity : wallColliders)
	{
		entity.updateCollider();
	}
	ECS::GetInstance().getComponent<ColliderComponent>(wallEntities[0]) = wallColliders[0];
	ECS::GetInstance().getComponent<ColliderComponent>(wallEntities[1]) = wallColliders[1];

	ECS::GetInstance().getComponent<transformComponent>(ground) = mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(ground) = mCollider;
}