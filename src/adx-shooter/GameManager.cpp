#include "pch.h"
#include "GameManager.h"
#include "adx-core/Timer.h"
#include <DirectXMath.h>
#include <random>
#include "MeshCreator.h"
#include <string>
#include "Rooms.h"

using namespace DirectX;
GameManager* GameManager::instance = nullptr;

GameManager::GameManager(HINSTANCE hInstance, int winW, int winH)
{
    instance = this;
    instance->mhInstance = hInstance;

}

bool GameManager::Initialize()
{
    //Initialize window
    {
        mWindow = new Window(mhInstance);
        mWindow->Initialize(1920, 1080);
    }
    mPlayer = new Player();
    mLifeTextRenderer = new TextRenderer(mWindow);
    mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mManaTextRenderer = new TextRenderer(mWindow);
    mManaTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mScoreTextRenderer = new TextRenderer(mWindow);
    mScoreTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mTimerTextRenderer = new TextRenderer(mWindow);
    mTimerTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mBossNameTextRenderer = new TextRenderer(mWindow);
    mBossNameTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mBossName2TextRenderer = new TextRenderer(mWindow);
    mBossName2TextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);

    srand(time(NULL));
    ecs = ECS::GetInstance();

    //Generate player
    {
        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, mPlayer->mEntity, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"Diamond2.dds");
        mEntityMesh.insert({ mPlayer->mEntity, playerMesh });
		MeshGeometry weaponMesh = MeshCreator::CreateBox(mWindow, mPlayer->mWeapon->GetEntity(), 1, 0.5f, 3, (XMFLOAT4)Colors::Red, L"Diamond2.dds");
		mEntityMesh.insert({ mPlayer->mWeapon->GetEntity(), weaponMesh });
    }
	mBulletMesh = MeshCreator::CreateBall(mWindow, 4, 1.0f, 10, 10, (XMFLOAT4)Colors::Blue);
	mLineBulletMesh = MeshCreator::CreateBox(mWindow, 5, 1, 1, 1, (XMFLOAT4)Colors::Blue, L"Diamond2.dds");
	mCircleMesh = MeshCreator::CreateCylinder(mWindow, 6, 10, 1, 1,30,10,(XMFLOAT4)Colors::Red);
    mEnemyMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

	currentRoom.Initialize(mWindow);
    mEntityMesh.insert({ currentRoom.ground, currentRoom.road });
    mEntityMesh.insert({ currentRoom.wall1Entity, currentRoom.wall1 });
    mEntityMesh.insert({ currentRoom.wall2Entity, currentRoom.wall2 });
    mEntityMesh.insert({ currentRoom.door.mEntity , currentRoom.door.doorMesh });
	currentRoom.door.mPlayer = mPlayer->mEntity;
    
	newBoss = new Makhina_Boss(mPlayer->mEntity);
    MakhinaBossMesh = MeshCreator::CreateBox(mWindow, newBoss->GetEntity(), 1,1,1, (XMFLOAT4)Colors::DarkRed);

    //Setup camera
    {
        mCamera.SetPosition(0.0f, 3.0f, -10.0f);
        float aspect = mWindow->AspectRatio();
        mCamera.SetLens(0.25f * XM_PI, aspect, 1.0f, 1000.0f);
        XMFLOAT3 pos = { 0.0f, 3.0f, -10.0f };
        XMVECTOR camPos = XMLoadFloat3(&pos);
        XMVECTOR target = XMVectorZero();
        mCamera.LookAt(camPos, target);
        mWindow->SetCamera(mCamera);
    }

    //Setup light
    {
        mLight = Light(XMFLOAT3(1.0f, -1.0f, 0.0f), 1, XMFLOAT4(1.0f, 1.f, 1.0f, 1.0f));
        mWindow->SetLight(mLight);
    }
    UIRenderer healthBarExtMesh(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");

    //Generate health bar
    {
		Entity healthExtBar = ecs.createEntity(transformComponent(offsetHBX, offsetHBY, 0, healthBarWidth, healthBarHeight));
		healthBarExtMesh.AddIndex(healthExtBar);
        healthBar = ecs.createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f));
        healthBossBar = ecs.createEntity(transformComponent(700, 70, 0, 500, 20));
        healthBossBar2 = ecs.createEntity(transformComponent(700, 170, 0, 500, 20));
		UIRenderer healthBarMesh(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		healthBarMesh.AddIndex(healthBar);
		healthBarMesh.AddIndex(healthBossBar);
		healthBarMesh.AddIndex(healthBossBar2);
		healthBarMesh.PushIndex();
        mUIMesh.insert({ healthExtBar, healthBarExtMesh.UIQuad });
		mHealthBarMesh = healthBarMesh;
        mUIMesh.insert({ healthBar, mHealthBarMesh.UIQuad });
		mUIMesh.insert({ healthBossBar, mHealthBarMesh.UIQuad });
		mUIMesh.insert({ healthBossBar2, mHealthBarMesh.UIQuad });
    }
    {
		Entity manaExtBar = ecs.createEntity(transformComponent(offsetMBX, offsetMBY, 0, healthBarWidth, healthBarHeight));
		healthBarExtMesh.AddIndex(manaExtBar);
		healthBarExtMesh.PushIndex();
        mUIMesh.insert({ manaExtBar, healthBarExtMesh.UIQuad });
        manaBar = ecs.createEntity(transformComponent(offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f));
        UIRenderer healthBarMesh(XMFLOAT4(0,0,1,1));
		healthBarMesh.AddIndex(manaBar);
		healthBarMesh.PushIndex();
        mUIMesh.insert({ manaBar, healthBarMesh.UIQuad });
    }
    mWindow->ExecuteInitCommands();
    mWindow->FlushCommandQueue();
    return true;
}

void GameManager::Update()
{
    float deltaTime = GetDeltatime();

	// Update du joueur
    mPlayer->Aim();

	// Update de la caméra pour suivre le joueur
	UpdateCam();
    // Update de la barre de vie UI
    UpdateBar();

	// Update des matrices des objets
    UpdateMatrix();

    // Tir (Bullet instantiation)
    Shoot();

	// Update des bullets et des ennemis
	BulletUpdate();
    EnemyUpdate();

	currentRoom.door.Update(mEnemyList.size() + mBossList.size());

    // Nettoyage final des entités supprimées cette frame
    Destroy();
}

bool GameManager::Run()
{
    Timer::Reset();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Timer::Update();
            Pause();
            if (!mAppPaused)
            {
                mWindow->CalculateFrameStats();
                Update();
                Draw();
            }
            else
            {
                Sleep(100);
            }
        }
    }

    return (int)msg.wParam;
}

void GameManager::Draw()
{
    mWindow->BeginFrame();

    for (auto it = mEntityMesh.begin(); it != mEntityMesh.end(); ++it)
    {
        int entityID = it->first;
        MeshGeometry& meshRef = it->second;

        mWindow->Draw(meshRef, entityID);
    }
    for (auto it = mUIMesh.begin(); it != mUIMesh.end(); ++it)
    {
        int entityID = it->first;
        MeshGeometry& meshRef = it->second;

        mWindow->DrawUI(meshRef, entityID);
    }
	timer += Timer::GetInstance()->GetDeltatime();
    int hours = (int)timer / 3600;
    int minutes = ((int)timer % 3600) / 60;
    int seconds = (int)timer % 60;
    int centiemes = (int)(timer * 100) % 100;

    std::string timerStr = std::format("Timer : {:02}:{:02}:{:02}.{:02}",
        hours, minutes, seconds, centiemes); 
    {
		mManaTextRenderer->DrawTxt(std::to_string((int)mPlayer->GetStats().mMana) + "/" + std::to_string((int)mPlayer->GetStats().mMana), offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f, 24);
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->GetStats().mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->GetHealth() > 0 ? std::to_string((int)mPlayer->GetHealth()) + "/" + std::to_string((int)mPlayer->GetStats().mHealth) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
        mTimerTextRenderer->DrawTxt(timerStr, 20, 60, 24);
        if (mBossList.size() > 0)
        {
            int size = mBossList[0]->GetName().size();
            if (size < 22)
                size = 22;
            else
                size = 500 / size;
            mBossNameTextRenderer->DrawTxt(mBossList[0]->GetName(), 700, 50, size);
            if (mBossList.size() > 1)
            {
				size = mBossList[1]->GetName().size();
				if (size < 22)
					size = 22;
				else
					size = 500 / size;
                mBossName2TextRenderer->DrawTxt(mBossList[1]->GetName(), 700, 110, size);
            }
        }
        else
        {
            mBossNameTextRenderer->DrawTxt("", 650, 10, 24);
            mBossName2TextRenderer->DrawTxt("", 650, 110, 24);
        }

    }

    firstFrame = false;
    mWindow->EndFrame();
}

void GameManager::Pause()
{
    if (mPlayer->GetHealth() <= 0) {
        mAppPaused = true;
    }

	static bool spaceDown = false;
    float playerHealth = mPlayer->GetHealth();
    if (playerHealth <= 0)
    {
        spaceDown = true;
        if (spaceDown != spaceDownLastFrame)
        {
            mAppPaused = !mAppPaused;
        }
        spaceDownLastFrame = true;
        spaceDown = false;
    }
    else
    {
        spaceDownLastFrame = false;
    }
    CheckInput();
}

void GameManager::Shoot()
{
}

void GameManager::SpawnBoss(float x, float z) {

    newBoss->GetTransform() = ecs.getComponent<transformComponent>(newBoss->GetEntity());
    newBoss->GetTransform().position = FLOAT3(x, newBoss->GetTransform().scale.y/2, z);
    mWindow->RegisterExistingMeshForEntity(newBoss->GetEntity());
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newBoss->GetEntity()));
    mEntityMesh.insert({ newBoss->GetEntity(), MakhinaBossMesh });
    mWindow->Update(newBoss->GetEntity(), enemyWorld);
    mBossList.push_back(newBoss);
}

void GameManager::CheckInput()
{
    if (InputSystem::isKeyDown(VK_F1) && mAppPaused)
    {
        HealthSystem::RecoverHealth(mPlayer->GetHealthComponent(), mPlayer->GetHealthComponent().mMaxHealth);
        mAppPaused = !mAppPaused;
        for (Bullet* bullet : mBulletList) {
            bullet->toBeDestroyed = true;
        }
        Destroy();
    }
    if (InputSystem::isKeyDown('C'))
    {
        SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
    }
	if (InputSystem::isKeyUp('E') && mEnemyList.size() <= 0 && mBossList.size() <= 0 && currentRoom.door.mCollider.collisionCheck(mPlayer->mEntity))
    {
        GenerateRoom();
    }
}

void GameManager::GenerateRoom()
{
    //mWindow->RemoveEntityResources(currentRoom.ground);
    mWindow->RemoveEntityResources(currentRoom.door.mEntity);
    currentRoom.Initialize(mWindow);
	int color = rand() % 5;
    if (mDoorOpened == 5) {
        color = 5;
		mDoorOpened = 0;
	}
    else
    {
        mDoorOpened++;
    }
    //if (currentRoom.generated == false)
    //{
    //    currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
    //    currentRoom.generated = true;
    //}
    mPlayer->GetTransform().position = FLOAT3(0, 2, -45);
    switch (color)
    {
    case 0:
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
        //currentRoom.door.mTransform.position = FLOAT3(0, 0, 50);
        for (int i = 0; i < 3; i++) {
            SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
        }
        break;
    case 1:
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::DarkGreen);
        //currentRoom.door.mTransform.position = FLOAT3(0, 0, 50);
        for (int i = 0; i < 7; i++) {
            SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
        }
        break;
    case 2:
        //currentRoom.door.mTransform.position = FLOAT3(50, 0, 50);
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::DarkBlue);
        for (int i = 0; i < 5; i++) {
            SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
        }
        break;
    case 3:
        //currentRoom.door.mTransform.position = FLOAT3(0, 0, 0);
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::DarkRed);
        for (int i = 0; i < 20; i++) {
            SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
        }
        break;
    case 4:
        //currentRoom.door.mTransform.position = FLOAT3(50, 0, -50);
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::DarkViolet);
        break;
    case 5:
        //currentRoom.door.mTransform.position = FLOAT3(-50, 0, -50);
        currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::DarkCyan);
        for (int i = 0; i < 1; i++) {
            SpawnBoss(0, 0);
        }
        break;
    default:
        //currentRoom.door.mTransform.position = FLOAT3(0, 0, 25);
        for (int i = 0; i < 10; i++) {
            SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
        }
        //currentRoom.road = MeshCreator::CreateBox(mWindow, currentRoom.ground, 100.0f, 1, 100, (XMFLOAT4)Colors::SkyBlue);
        break;
    }

    mEntityMesh.insert({ currentRoom.ground, currentRoom.road });
}

//
float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::UpdateCam()
{
    transformComponent& playerTrans = mPlayer->GetTransform();
    mCamera.SetPosition(toXMFLOAT3(playerTrans.position + FLOAT3(30, 30, -30)));
    XMFLOAT3 playerPos = toXMFLOAT3(playerTrans.position);
    XMVECTOR targetVect = XMLoadFloat3(&playerPos);
    XMFLOAT3 camPos = mCamera.Position();
    mCamera.LookAt(XMLoadFloat3(&camPos), targetVect);
    mWindow->SetCamera(mCamera);

    static bool OneDownLastFrame = false;
    if (InputSystem::isKeyDown('1')) {
        if (!OneDownLastFrame) {
            mPlayer->ChangeAimType();
            OneDownLastFrame = true;
        }
    }
    else {
        OneDownLastFrame = false;
    }
}

void GameManager::EnemyUpdate()
{
    for (EnemyMarksman* enemy : mEnemyList) {
        enemy->Update();
    }
    for (Boss* boss : mBossList) {
        boss->Update();
    }
}

void GameManager::BulletUpdate()
{
    for (int i = (int)mPlayerbulletList.size() - 1; i >= 0; i--) {
        Bullet* bullet = mPlayerbulletList[i];
        bullet->Update();

        if (!bullet->toBeDestroyed) {
            float maxS = max(bullet->mTransform.scale.x, max(bullet->mTransform.scale.z, bullet->mTransform.scale.y));
            float thresholdSq = (maxS + 3.0f) * (maxS + 3.0f);
            if (bullet->currentLifetime >= bullet->maxLifetime) {
                bullet->toBeDestroyed = true;
            }

            for (EnemyMarksman* enemy : mEnemyList) {
                if (enemy->isDead) continue;
                if (bullet->entitiesToIgnore.count(enemy->mEntity)) continue;
                float dx = bullet->mTransform.position.x - enemy->GetTransform().position.x;
                float dz = bullet->mTransform.position.z - enemy->GetTransform().position.z;
                float distSq = (dx * dx + dz * dz);
                if (distSq < thresholdSq) {
                    if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(enemy->mEntity)) {
                        enemy->TakeDamage(bullet->mDamage);
                        bullet->entitiesToIgnore[enemy->mEntity] = true;
                        if (!enemy->IsAlive()) {
                            mDestroyEnemyList.push_back(enemy);

                            mPlayer->GetStats().mExp += 10;
                        }
                        if (!bullet->isPersistantBullet) {
                            bullet->toBeDestroyed = true;
                            break;
                        }
                        if (bullet->isBoucingBullet)
                        {
                            if(bullet->allowedBounces > 0)
                            {
                                bullet->allowedBounces--;
								bullet->currentLifetime -= 0.25f;
								float closestDistance = FLT_MAX;
                                for (EnemyMarksman* enemy : mEnemyList) {
                                    if (enemy->isDead) continue;
                                    if (bullet->entitiesToIgnore.count(enemy->mEntity)) continue;
                                    float distance = sqrt(pow(bullet->mTransform.position.x - enemy->GetTransform().position.x, 2) + pow(bullet->mTransform.position.z - enemy->GetTransform().position.z, 2));
                                    if (distance < closestDistance) {
                                        closestDistance = distance;
                                    }
                                    else
                                    {
                                        continue;
                                    }
                                    if (distance < 70) {
                                        float dx = enemy->GetTransform().position.x - bullet->mTransform.position.x;
                                        float dz = enemy->GetTransform().position.z - bullet->mTransform.position.z;
                                        float angle = atan2f(dx, dz);
                                        bullet->mTransform.rotation.y = angle;
                                    }
                                }
                            }
                            else
                            {
                                bullet->toBeDestroyed = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (!bullet->toBeDestroyed) {
                for (Boss* boss : mBossList) {
                    if (!boss->IsAlive() || bullet->entitiesToIgnore.count(boss->GetEntity())) continue;
                    float dx = bullet->mTransform.position.x - boss->GetTransform().position.x;
                    float dz = bullet->mTransform.position.z - boss->GetTransform().position.z;
					thresholdSq = (maxS + 10.0f) * (maxS + 10.0f);
                    if ((dx * dx + dz * dz) < thresholdSq) {
                        if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(boss->GetEntity())) {
                            boss->TakeDamage(bullet->mDamage);
                            bullet->entitiesToIgnore[boss->GetEntity()] = true;
                            if (!boss->IsAlive()) {
                                mDestroyBossList.push_back(boss);
                                boss = mBossList.back();
                                mBossList.pop_back();

                                mPlayer->GetStats().mExp += 50;
                            }
                            if (!bullet->isPersistantBullet) { bullet->toBeDestroyed = true; break; }
                        }
                    }
                }
            }
        }
        if (bullet->isBomb)
        {
            if (bullet->toBeDestroyed)
            {
                Shot* newShot = Shoot_Pattern_Explosion::Shoot(bullet->mEntity, 9, bullet->bombDamage, 1, 50);
                for (int i = 0; i < newShot->bulletList.size(); ++i)
                {
                    mEntityMesh.insert({ newShot->bulletList[i]->mEntity, mBulletMesh });
                    mPlayerbulletList.push_back(newShot->bulletList[i]);
                    GetWindow()->RegisterExistingMeshForEntity(newShot->bulletList[i]->mEntity);
                }

            }
        }

        if (mPlayerbulletList[i]->toBeDestroyed) {
            mDestroyBulletList.push_back(mPlayerbulletList[i]);
            mPlayerbulletList[i] = mPlayerbulletList.back();
            mPlayerbulletList.pop_back();
        }
    }
    for (int i = mBulletList.size() - 1; i >= 0; i--) {
        mBulletList[i]->Update();
		Bullet* bullet = mBulletList[i];
        if (!bullet->toBeDestroyed) {
            if (bullet->entitiesToIgnore.count(mPlayer->mEntity)) continue;
            if (bullet->currentLifetime >= bullet->maxLifetime) {
                bullet->toBeDestroyed = true;
            }

            float maxS = max(bullet->mTransform.scale.x, max(bullet->mTransform.scale.z, bullet->mTransform.scale.y));
            float thresholdSq = (maxS + 3.0f) * (maxS + 3.0f);
            float dx = bullet->mTransform.position.x - mPlayer->GetTransform().position.x;
            float dz = bullet->mTransform.position.z - mPlayer->GetTransform().position.z;
            if ((dx * dx + dz * dz) < thresholdSq) {
                if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(mPlayer->mEntity)) {
                    mPlayer->takeDamage(bullet->mDamage);
                    bullet->entitiesToIgnore[mPlayer->mEntity] = true;
                    if (!bullet->isPersistantBullet) {
                        bullet->toBeDestroyed = true;
                        break;
                    }
                }
            }
        }

        if (mBulletList[i]->toBeDestroyed) {
            mDestroyBulletList.push_back(mBulletList[i]);
            mBulletList.erase(mBulletList.begin() + i);
        }
    }
}

void GameManager::UpdateMatrix()
{
    for (auto it = mEntityMesh.begin(); it != mEntityMesh.end(); ++it)
    {
        int entityID = it->first;
        XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(entityID));
        mWindow->Update(entityID, entityWorld);
    }
    for (auto it = mUIMesh.begin(); it != mUIMesh.end(); ++it)
    {
        int entityID = it->first;
        XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(entityID));
        mWindow->UpdateUI(entityID, entityWorld);
    }
}

void GameManager::UpdateBar()
{
    ecs.getComponent<transformComponent>(healthBar).scale.x = (mPlayer->GetHealth() / mPlayer->GetStats().mHealth) * healthBarWidth *0.9;
	ecs.getComponent<transformComponent>(healthBar).scale.y = healthBarHeight * 0.35f;
	if (mBossList.size() > 0)
    {
        ecs.getComponent<transformComponent>(healthBossBar).scale.x = (mBossList[0]->GetHealth() / mBossList[0]->GetMaxHealth()) * 500;
        if (mBossList.size() > 1)
        {
            ecs.getComponent<transformComponent>(healthBossBar2).scale.x = (mBossList[1]->GetHealth() / mBossList[1]->GetMaxHealth()) * 500;
        }
    }
    else
    {
        ecs.getComponent<transformComponent>(healthBossBar).scale.x = 0;
        ecs.getComponent<transformComponent>(healthBossBar2).scale.x = 0;
	}
}

void GameManager::Destroy() {
    if (!mDestroyBulletList.empty()) {
        for (Bullet* bullet : mDestroyBulletList) {
            mWindow->RemoveEntityResources(bullet->mEntity);

            mEntityMesh.erase(bullet->mEntity);

            delete bullet;

        }
        mDestroyBulletList.clear();
    }

    if (!mDestroyEnemyList.empty()) {
        for (EnemyMarksman* enemy : mDestroyEnemyList) {
            auto it = std::find(mEnemyList.begin(), mEnemyList.end(), enemy);
            if (it != mEnemyList.end()) {
                mEnemyList.erase(it);
            }

            mWindow->RemoveEntityResources(enemy->mEntity);

            mEntityMesh.erase(enemy->mEntity);

            delete enemy;
        }
        mDestroyEnemyList.clear();
    }
    if (!mDestroyBossList.empty()) {
        for (Boss* boss : mDestroyBossList) {
            auto it = std::find(mBossList.begin(), mBossList.end(), boss);
            if (it != mBossList.end()) {
                mBossList.erase(it);
            }
            mWindow->RemoveEntityResources(boss->GetEntity());
            mEntityMesh.erase(boss->GetEntity());
            delete boss;
        }
        mDestroyBossList.clear();
    }
}

void GameManager::SpawnMob(float x, float z, int mob) {
    EnemyMarksman* newEnemy = new EnemyMarksman();
    newEnemy->Init(mPlayer->mEntity, mWindow);

    newEnemy->GetTransform() = ecs.getComponent<transformComponent>(newEnemy->mEntity);
    newEnemy->GetTransform().position = FLOAT3(x, 2, z);

    mEntityMesh.insert({ newEnemy->mEntity, mEnemyMesh });

    mWindow->RegisterExistingMeshForEntity(newEnemy->mEntity);
    mWindow->RegisterExistingMeshForEntity(newEnemy->healthBar);

    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newEnemy->mEntity));
    mWindow->Update(newEnemy->mEntity, enemyWorld);

    mEnemyList.push_back(newEnemy);
}