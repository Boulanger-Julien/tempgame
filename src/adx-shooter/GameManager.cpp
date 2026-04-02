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

    mhInstance = hInstance;
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
}

bool GameManager::Initialize()
{
    srand(time(NULL));
    ecs = ECS::GetInstance();

    //Generate player
    {
        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, mPlayer->mEntity, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"Diamond2.dds");
        mEntityMesh.insert({ mPlayer->mEntity, playerMesh });
		MeshGeometry weaponMesh = MeshCreator::CreateBox(mWindow, mPlayer->mWeapon->GetEntity(), 1, 0.5f, 3, (XMFLOAT4)Colors::Red, L"Diamond2.dds");
		mEntityMesh.insert({ mPlayer->mWeapon->GetEntity(), weaponMesh });
    }
	Shoot_Pattern_Explosion::GetInstance().SetPlayerIndex(mPlayer->mEntity);
	Shoot_Pattern_Single_Shot::GetInstance().SetPlayerIndex(mPlayer->mEntity);
	Shoot_Pattern_Line::GetInstance().SetPlayerIndex(mPlayer->mEntity);
	mBulletMesh = MeshCreator::CreateBall(mWindow, 4, 1.0f, 10, 10, (XMFLOAT4)Colors::Blue);
	mLineBulletMesh = MeshCreator::CreateBox(mWindow, 5, 1, 1, 1, (XMFLOAT4)Colors::Blue, L"Diamond2.dds");
    mEnemyMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

	currentRoom.Initialize(mWindow);

	currentRoom.door.mPlayer = mPlayer->mEntity;
    
	newBoss = new Makhina_Boss(mPlayer->mEntity);
    MakhinaBossMesh = MeshCreator::CreateBox(mWindow, newBoss->GetEntity(), 1,1,1, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

    Entity cloud = ecs.createEntity(transformComponent(0, 10, 0));
    MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\Json\\Cloud.json", 1000, (XMFLOAT4)Colors::White);
    mEntityMesh.insert({ cloud, cloudMesh });
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

    //Generate health bar
    {
        Entity healthExtBar = ecs.createEntity(transformComponent(offsetHBX, offsetHBY));
        UIRenderer healthBarExtMesh(*mWindow, healthExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
        mUIMesh.insert({ healthExtBar, healthBarExtMesh.UIQuad });
        healthBar = ecs.createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f));
		healthBossBar = ecs.createEntity(transformComponent(700, 70, 0, 500, 20));
		healthBossBar2 = ecs.createEntity(transformComponent(700, 170, 0, 500, 20));
        UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
        UIRenderer healthBarBossMesh(*mWindow, healthBossBar, 1, 1, XMFLOAT4(Colors::Red));
        UIRenderer healthBarBoss2Mesh(*mWindow, healthBossBar2, 1, 1, XMFLOAT4(Colors::Red));
        mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
		mUIMesh.insert({ healthBossBar, healthBarBossMesh.UIQuad });
		mUIMesh.insert({ healthBossBar2, healthBarBoss2Mesh.UIQuad });
    }
    {
        Entity manaExtBar = ecs.createEntity(transformComponent(offsetMBX, offsetMBY));
        UIRenderer healthBarExtMesh(*mWindow, manaExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
        mUIMesh.insert({ manaExtBar, healthBarExtMesh.UIQuad });
        manaBar = ecs.createEntity(transformComponent(offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f));
        UIRenderer healthBarMesh(*mWindow, manaBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Blue));
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

	//currentRoom.door.Update(mEnemyList.size() + mBossList.size());
	currentRoom.Update();

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

    // Draw all entities
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

    {
        currentRoom.Draw();
		mManaTextRenderer->DrawTxt(0/*std::to_string((int)mPlayer->GetStats().mManaPoints)*/ + "/" + std::to_string((int)mPlayer->GetStats().mMana), offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f, 24);
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->GetStats().mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->GetHealth() > 0 ? std::to_string((int)mPlayer->GetHealth()) + "/" + std::to_string((int)mPlayer->GetStats().mHealth) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
        //currentRoom.mNumberOfRoomRenderer->DrawTxt("Rooggegmy", 60, 220, 24);

        if (mBossList.size() > 0)
        {
            int size = mBossList[0]->GetName().size();
            if (size < 22)
                size = 22;
            else
                size = 500 / size;
            mBossNameTextRenderer->DrawTxt(mBossList[0]->GetName(), 700, 10, size);
            if (mBossList.size() > 1)
            {
				size = mBossList[1]->GetName().size();
				if (size < 22)
					size = 22;
				else
					size = 500 / size;
                mBossName2TextRenderer->DrawTxt(mBossList[1]->GetName(), 700, 110, 24);
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
	static bool spaceDown = false;
    // Toggle pause when F1 is pressed
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

/////////////////////////

void GameManager::Shoot()
{
    static bool cDownLastFrame = false;
    static bool cDownLastFrame2 = false;
	static bool cDownLastFrame3 = false;
	static bool cDownLastFrame4 = false;
    if (InputSystem::isKeyDown(VK_LBUTTON))
    {
        if (!cDownLastFrame) {
            mPlayer->AddBullet();
            cDownLastFrame = true;
        }
    }
    else {
        cDownLastFrame = false;
    }
    if (InputSystem::isKeyDown(VK_SPACE)) {
        if (!cDownLastFrame2) {
            mPlayer->AddExplosionBullet();
            cDownLastFrame2 = true;
        }
    }
    else {
        cDownLastFrame2 = false;
    }
    if (InputSystem::isKeyDown(VK_RBUTTON))
    {
        if (!cDownLastFrame3) {
            mPlayer->AddLineBullet();
            cDownLastFrame3 = true;
        }
    }
    else {
        cDownLastFrame3 = false;
    }
    if (InputSystem::isKeyDown('W')) {
        if (!cDownLastFrame4) {
            mPlayer->TestShootPattern();
            cDownLastFrame4 = true;
        }
    }
    else {
        cDownLastFrame4 = false;
	}
}

//?
Boss* GameManager::SpawnBoss(float x, float z) {
    newBoss = new Makhina_Boss(mPlayer->mEntity);

    newBoss->GetTransform() = ecs.getComponent<transformComponent>(newBoss->GetEntity());
    newBoss->GetTransform().position = FLOAT3(x, newBoss->GetTransform().scale.y/2, z);
    mWindow->RegisterExistingMeshForEntity(newBoss->GetEntity());
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newBoss->GetEntity()));
    mEntityMesh.insert({ newBoss->GetEntity(), MakhinaBossMesh });
    mWindow->Update(newBoss->GetEntity(), enemyWorld);
    mBossList.push_back(newBoss);
	return newBoss;
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

}

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
    for (Enemy* enemy : mEnemyList) {
        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
    }
    //Julien
    for (Boss* boss : mBossList) {
        boss->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
    }
}

void GameManager::BulletUpdate()
{
    for (int i = (int)mPlayerbulletList.size() - 1; i >= 0; i--) {
        Bullet* bullet = mPlayerbulletList[i];
        bullet->Update();
        for (Entity entity : currentRoom.wallEntities)
        {
            if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(entity))
            {
                if (bullet->type == BULLET)
                bullet->toBeDestroyed = true;
                else {
                    bullet->mTransform.scale.z -= 2;
                    transformSystem::MoveForward(ECS::GetInstance().getComponent<transformComponent>(bullet->mEntity), -1);
                }
            }
        }

        if (!bullet->toBeDestroyed) {
            float maxS = max(bullet->mTransform.scale.x, max(bullet->mTransform.scale.z, bullet->mTransform.scale.y));
            float thresholdSq = (maxS + 3.0f) * (maxS + 3.0f);
            for (Enemy* enemy : mEnemyList) {
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
                    }
                }
            }

            if (!bullet->toBeDestroyed) {
                for (Boss* boss : mBossList) {
                    if (!boss->IsAlive() || bullet->entitiesToIgnore.count(boss->GetEntity())) continue;

                    float dx = bullet->mTransform.position.x - boss->GetTransform().position.x;
                    float dz = bullet->mTransform.position.z - boss->GetTransform().position.z;
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
    ecs.getComponent<transformComponent>(healthBar).scale.x = mPlayer->GetHealth() / mPlayer->GetStats().mHealth;
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
    // NETTOYAGE DES BULLETS
    if (!mDestroyBulletList.empty()) {
        for (Bullet* bullet : mDestroyBulletList) {
            // 1. Libérer les ressources DirectX (Slots de descripteurs)
            mWindow->RemoveEntityResources(bullet->mEntity);

            // 2. Retirer du système de rendu
            mEntityMesh.erase(bullet->mEntity);

            // 4. Supprimer l'objet C++
            delete bullet;

        }
        mDestroyBulletList.clear();
    }

    // NETTOYAGE DES ENNEMIS (Même logique)
    if (!mDestroyEnemyList.empty()) {
        for (Enemy* enemy : mDestroyEnemyList) {
            // 1. On le retire de la liste de l'Update LOGIQUE
            auto it = std::find(mEnemyList.begin(), mEnemyList.end(), enemy);
            if (it != mEnemyList.end()) {
                mEnemyList.erase(it);
            }

            // 2. On retire les ressources DirectX
            mWindow->RemoveEntityResources(enemy->mEntity);

            // 3. On le retire du dictionnaire de rendu
            mEntityMesh.erase(enemy->mEntity);

            // 4. Supprimer l'objet C++
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

Enemy* GameManager::SpawnMob(float x, float z, int mob) {
    EnemyMarksman* newEnemy = new EnemyMarksman();
    newEnemy->Init(mPlayer->mEntity);
    newEnemy->GetTransform() = ecs.getComponent<transformComponent>(newEnemy->mEntity);
    newEnemy->GetTransform().position = FLOAT3(x, 2, z);
    mWindow->RegisterExistingMeshForEntity(newEnemy->mEntity);
    mEntityMesh.insert({ newEnemy->mEntity, mEnemyMesh });
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newEnemy->mEntity));
    mWindow->Update(newEnemy->mEntity, enemyWorld);
	mEnemyList.push_back(newEnemy);
    return newEnemy;
}
