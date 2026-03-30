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
    MakhinaBossMesh = MeshCreator::CreateBox(mWindow, newBoss->GetEntity(), newBoss->GetTransform().scale.x, newBoss->GetTransform().scale.x, newBoss->GetTransform().scale.x, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

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
        UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
        mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
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
    ecs.getComponent<transformComponent>(healthBar).scale.x = mPlayer->GetHealth() / mPlayer->GetStats().mHealth;

	// Update des matrices des objets
    UpdateMatrix();

    // Tir (Bullet instantiation)
    Shoot();

	// Update des bullets et des ennemis
	BulletUpdate();
    EnemyUpdate();

	currentRoom.door.Update(mEnemyList.size() + mBossList.size());
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
    int hours = (int)timer / 3600;
    int minutes = ((int)timer % 3600) / 60;
    int seconds = (int)timer % 60;
    int centiemes = (int)(timer * 100) % 100;

    std::string timerStr = std::format("Timer : {:02}:{:02}:{:02}.{:02}",
        hours, minutes, seconds, centiemes); 
    {
		mManaTextRenderer->DrawTxt(0/*std::to_string((int)mPlayer->GetStats().mManaPoints)*/ + "/" + std::to_string((int)mPlayer->GetStats().mMana), offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f, 24);
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->GetStats().mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->GetHealth() > 0 ? std::to_string((int)mPlayer->GetHealth()) + "/" + std::to_string((int)mPlayer->GetStats().mHealth) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
        mTimerTextRenderer->DrawTxt(timerStr, 20, 60, 24);
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

void GameManager::AddLineBullet(Entity sender, float _damage)
{
    Bullet* newBullet = Shoot_Pattern_Line::Shoot(sender, _damage, 25, 1, mWindow);
    mWindow->RegisterExistingMeshForEntity(newBullet->mEntity);
    mEntityMesh.insert({ newBullet->mEntity, mLineBulletMesh });
    XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newBullet->mEntity));
    mWindow->Update(newBullet->mEntity, bulletWorld);
    if (sender == mPlayer->mEntity) {
        newBullet->mDamage = _damage;
        mPlayerbulletList.push_back(newBullet);
    }
    else
    {
        newBullet->mDamage = _damage;
        mBulletList.push_back(newBullet);
    }
}

void GameManager::AddExplosionBullet(Entity sender, float bullets)
{
	Shot* newShot = Shoot_Pattern_Explosion::Shoot(sender, bullets, mPlayer->GetStats().mStrength, mWindow);
	for (int i = 0; i < newShot->bulletList.size(); ++i)
    {
		mEntityMesh.insert({ newShot->bulletList[i]->mEntity, mBulletMesh });
        mPlayerbulletList.push_back(newShot->bulletList[i]);
    }
}

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
            AddExplosionBullet(mPlayer->mEntity, 9);
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
void GameManager::SpawnBoss(float x, float z) {
    newBoss = new Makhina_Boss(mPlayer->mEntity);

    newBoss->GetTransform() = ecs.getComponent<transformComponent>(newBoss->GetEntity());
    newBoss->GetTransform().position = FLOAT3(x, 13.5, z);
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
	if (InputSystem::isKeyUp('E') && currentRoom.door.changeRoom == true)
    {
        GenerateRoom();
    }
}

void GameManager::GenerateRoom()
{
    //mWindow->RemoveEntityResources(currentRoom.ground);
    mWindow->RemoveEntityResources(currentRoom.door.mEntity);
    currentRoom.Initialize(mWindow);
    int color = rand() % 6;
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
            SpawnBoss(rand() % 100 - 50, rand() % 100 - 50);
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
        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
    }
    //Julien
    for (Boss* boss : mBossList) {
        transformComponent& bossTransform = ECS::GetInstance().getComponent<transformComponent>(boss->GetEntity());
        boss->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
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
    //ecs.getComponent<transformComponent>(manaBar).scale.x = 0/*mPlayer->GetStats().mMana*/ / mPlayer->GetStats().mMana;
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
        for (EnemyMarksman* enemy : mDestroyEnemyList) {
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
            // 1. On le retire de la liste de l'Update LOGIQUE
            auto it = std::find(mBossList.begin(), mBossList.end(), boss);
            if (it != mBossList.end()) {
                mBossList.erase(it);
            }
            // 2. On retire les ressources DirectX
            mWindow->RemoveEntityResources(boss->GetEntity());
            // 3. On le retire du dictionnaire de rendu
            mEntityMesh.erase(boss->GetEntity());
            // 4. Supprimer l'objet C++
            delete boss;
        }
        mDestroyBossList.clear();
    }
}

void GameManager::SpawnMob(float x, float z, int mob) {
    EnemyMarksman* newEnemy = new EnemyMarksman();
    newEnemy->Init(mPlayer->mEntity);
    newEnemy->GetTransform() = ecs.getComponent<transformComponent>(newEnemy->mEntity);
    newEnemy->GetTransform().position = FLOAT3(x, 2, z);
    mWindow->RegisterExistingMeshForEntity(newEnemy->mEntity);
    mEntityMesh.insert({ newEnemy->mEntity, mEnemyMesh });
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newEnemy->mEntity));
    mWindow->Update(newEnemy->mEntity, enemyWorld);
	mEnemyList.push_back(newEnemy);
}
