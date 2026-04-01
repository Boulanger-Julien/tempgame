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

GameManager::GameManager()
{
    instance = this;

}

bool GameManager::Initialize()
{
    mPlayer = new Player();

    srand(time(NULL));
//    ecs = ECS::GetInstance();
//    //mLifeTextRenderer = new TextRenderer(mWindow);
//    //mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
//    //mManaTextRenderer = new TextRenderer(mWindow);
//    //mManaTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
//    //mScoreTextRenderer = new TextRenderer(mWindow);
//    //mScoreTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
//    //mTimerTextRenderer = new TextRenderer(mWindow);
//    //mTimerTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
//
//    //Generate player
//    {
//        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, mPlayer->mEntity, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"Diamond2.dds");
//        mEntityMesh.insert({ mPlayer->mEntity, playerMesh });
//		MeshGeometry weaponMesh = MeshCreator::CreateBox(mWindow, mPlayer->mWeapon->GetEntity(), 1, 0.5f, 3, (XMFLOAT4)Colors::Red, L"Diamond2.dds");
//		mEntityMesh.insert({ mPlayer->mWeapon->GetEntity(), weaponMesh });
//    }
//	Shoot_Pattern_Explosion::GetInstance().SetPlayerIndex(mPlayer->mEntity);
//	Shoot_Pattern_Single_Shot::GetInstance().SetPlayerIndex(mPlayer->mEntity);
//	Shoot_Pattern_Line::GetInstance().SetPlayerIndex(mPlayer->mEntity);
//	mBulletMesh = MeshCreator::CreateBall(mWindow, 4, 1.0f, 10, 10, (XMFLOAT4)Colors::Blue);
//	mLineBulletMesh = MeshCreator::CreateBox(mWindow, 5, 1, 1, 1, (XMFLOAT4)Colors::Blue, L"Diamond2.dds");
//    mEnemyMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");
//    mFireBallMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::Green, L"Diamond2.dds");
//
//	currentRoom.Initialize(mWindow);
//    mEntityMesh.insert({ currentRoom.ground, currentRoom.road });
//    mEntityMesh.insert({ currentRoom.wall1Entity, currentRoom.wall1 });
//    mEntityMesh.insert({ currentRoom.wall2Entity, currentRoom.wall2 });
//    mEntityMesh.insert({ currentRoom.door.mEntity , currentRoom.door.doorMesh });
//	currentRoom.door.mPlayer = mPlayer->mEntity;
//    
//	newBoss = new Makhina_Boss(mPlayer->mEntity);
//    MakhinaBossMesh = MeshCreator::CreateBox(mWindow, newBoss->GetEntity(), newBoss->GetTransform().scale.x, newBoss->GetTransform().scale.x, newBoss->GetTransform().scale.x, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");
//
//    Entity cloud = ecs.createEntity(transformComponent(0, 10, 0));
//    MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\Json\\Cloud.json", 1000, (XMFLOAT4)Colors::White);
//    mEntityMesh.insert({ cloud, cloudMesh });
//    //Setup camera
//    {
//        mCamera.SetPosition(0.0f, 3.0f, -10.0f);
//        float aspect = mWindow->AspectRatio();
//        mCamera.SetLens(0.25f * XM_PI, aspect, 1.0f, 1000.0f);
//        XMFLOAT3 pos = { 0.0f, 3.0f, -10.0f };
//        XMVECTOR camPos = XMLoadFloat3(&pos);
//        XMVECTOR target = XMVectorZero();
//        mCamera.LookAt(camPos, target);
//        mWindow->SetCamera(mCamera);
//    }
//
//    //Setup light
//    {
//        mLight = Light(XMFLOAT3(1.0f, -1.0f, 0.0f), 1, XMFLOAT4(1.0f, 1.f, 1.0f, 1.0f));
//        mWindow->SetLight(mLight);
//    }
//
//    //Generate health bar
//    {
//        Entity healthExtBar = ecs.createEntity(transformComponent(offsetHBX, offsetHBY));
//        UIRenderer healthBarExtMesh(*mWindow, healthExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
//        mUIMesh.insert({ healthExtBar, healthBarExtMesh.UIQuad });
//        healthBar = ecs.createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f));
//        UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
//        mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
//    }
//    {
//        Entity manaExtBar = ecs.createEntity(transformComponent(offsetMBX, offsetMBY));
//        UIRenderer healthBarExtMesh(*mWindow, manaExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
//        mUIMesh.insert({ manaExtBar, healthBarExtMesh.UIQuad });
//        manaBar = ecs.createEntity(transformComponent(offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f));
//        UIRenderer healthBarMesh(*mWindow, manaBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Blue));
//        mUIMesh.insert({ manaBar, healthBarMesh.UIQuad });
//    }
//
//    mWindow->ExecuteInitCommands();
//    mWindow->FlushCommandQueue();
    return true;
}
//
void GameManager::Update()
{
//    float deltaTime = GetDeltatime();
//
//	// Update du joueur
    mPlayer->Aim();
//
//	// Update de la caméra pour suivre le joueur
//	UpdateCam();
//    // Update de la barre de vie UI
//    UpdateBar();
//    ecs.getComponent<transformComponent>(healthBar).scale.x = mPlayer->GetHealth() / mPlayer->GetStats().mHealth;
//
//	// Update des matrices des objets
//    UpdateMatrix();
//
//	// Update des bullets et des ennemis
//	BulletUpdate();
//    EnemyUpdate();
//
//	currentRoom.door.Update(mEnemyList.size());
//
//    // Nettoyage final des entités supprimées cette frame
//    Destroy();
//}
//
//void GameManager::Destroy() {
//    // NETTOYAGE DES BULLETS
//    if (!mDestroyBulletList.empty()) {
//        for (Bullet* bullet : mDestroyBulletList) {
//            // 1. Libérer les ressources DirectX (Slots de descripteurs)
//            mWindow->RemoveEntityResources(bullet->mEntity);
//
//            // 2. Retirer du système de rendu
//            mEntityMesh.erase(bullet->mEntity);
//
//            // 4. Supprimer l'objet C++
//            delete bullet;
//        }
//        mDestroyBulletList.clear();
//    }
//
//    // NETTOYAGE DES ENNEMIS (Même logique)
//    if (!mDestroyEnemyList.empty()) {
//        for (EnemyMarksman* enemy : mDestroyEnemyList) {
//            // 1. On le retire de la liste de l'Update LOGIQUE
//            auto it = std::find(mEnemyList.begin(), mEnemyList.end(), enemy);
//            if (it != mEnemyList.end()) {
//                mEnemyList.erase(it);
//            }
//
//            // 2. On retire les ressources DirectX
//            mWindow->RemoveEntityResources(enemy->mEntity);
//
//            // 3. On le retire du dictionnaire de rendu
//            mEntityMesh.erase(enemy->mEntity);
//
//            // 4. Supprimer l'objet C++
//            delete enemy;
//        }
//        mDestroyEnemyList.clear();
//    }
//    if (!mDestroyBossList.empty()) {
//        for (Boss* boss : mDestroyBossList) {
//            // 1. On le retire de la liste de l'Update LOGIQUE
//            auto it = std::find(mBossList.begin(), mBossList.end(), boss);
//            if (it != mBossList.end()) {
//                mBossList.erase(it);
//            }
//            // 2. On retire les ressources DirectX
//            mWindow->RemoveEntityResources(boss->GetEntity());
//            // 3. On le retire du dictionnaire de rendu
//            mEntityMesh.erase(boss->GetEntity());
//            // 4. Supprimer l'objet C++
//            delete boss;
//        }
//        mDestroyBossList.clear();
//    }
}
//
//void GameManager::Pause()
//{
//    if (mPlayer->GetHealth() <= 0) {
//        mAppPaused = true;
//    }
//
//	static bool spaceDown = false;
//    // Toggle pause when F1 is pressed
//    float playerHealth = mPlayer->GetHealth();
//    if (playerHealth <= 0)
//    {
//        spaceDown = true;
//        if (spaceDown != spaceDownLastFrame)
//        {
//            mAppPaused = !mAppPaused;
//        }
//        spaceDownLastFrame = true;
//        spaceDown = false;
//    }
//    else
//    {
//        spaceDownLastFrame = false;
//    }
//    CheckInput();
//}
//
//void GameManager::SpawnBoss(float x, float z) {
//    newBoss = new Makhina_Boss(mPlayer->mEntity);
//
//    newBoss->GetTransform() = ecs.getComponent<transformComponent>(newBoss->GetEntity());
//    newBoss->GetTransform().position = FLOAT3(x, 13.5, z);
//    mWindow->RegisterExistingMeshForEntity(newBoss->GetEntity());
//    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newBoss->GetEntity()));
//    mEntityMesh.insert({ newBoss->GetEntity(), MakhinaBossMesh });
//    mWindow->Update(newBoss->GetEntity(), enemyWorld);
//    mBossList.push_back(newBoss);
//}
//
//void GameManager::CheckInput()
//{
//    if (InputSystem::isKeyDown(VK_F1) && mAppPaused)
//    {
//        HealthSystem::RecoverHealth(mPlayer->GetHealthComponent(), mPlayer->GetHealthComponent().mMaxHealth);
//        mAppPaused = !mAppPaused;
//        for (Bullet* bullet : mBulletList) {
//            bullet->toBeDestroyed = true;
//        }
//        Destroy();
//    }
//    if (InputSystem::isKeyDown('C'))
//    {
//        SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
//    }
//	if (InputSystem::isKeyUp('E') && mEnemyList.size() <= 0 && mBossList.size() <= 0 && currentRoom.door.mCollider.collisionCheck(mPlayer->mEntity))
//    {
//        GenerateRoom();
//    }
//}
//
//void GameManager::EnemyUpdate()
//{
//    for (EnemyMarksman* enemy : mEnemyList) {
//        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
//    }
//    //Julien
//    for (Boss* boss : mBossList) {
//        transformComponent& bossTransform = ECS::GetInstance().getComponent<transformComponent>(boss->GetEntity());
//        boss->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
//    }
//}
//
//void GameManager::BulletUpdate()
//{
//    for (auto& spell : mSpellListe)
//    {
//        spell->Update();
//    }
//    for (int i = (int)mPlayerbulletList.size() - 1; i >= 0; i--) {
//        Bullet* bullet = mPlayerbulletList[i];
//        bullet->Update();
//
//        if (!bullet->toBeDestroyed) {
//            float maxS = max(bullet->mTransform.scale.x, max(bullet->mTransform.scale.z, bullet->mTransform.scale.y));
//            float thresholdSq = (maxS + 3.0f) * (maxS + 3.0f);
//            for (EnemyMarksman* enemy : mEnemyList) {
//                if (enemy->isDead) continue;
//                if (bullet->entitiesToIgnore.count(enemy->mEntity)) continue;
//                float dx = bullet->mTransform.position.x - enemy->GetTransform().position.x;
//                float dz = bullet->mTransform.position.z - enemy->GetTransform().position.z;
//                float distSq = (dx * dx + dz * dz);
//                if (distSq < thresholdSq) {
//                    if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(enemy->mEntity)) {
//                        enemy->TakeDamage(bullet->mDamage);
//                        bullet->entitiesToIgnore[enemy->mEntity] = true;
//                        if (!enemy->IsAlive()) {
//                            mDestroyEnemyList.push_back(enemy);
//                            mPlayer->GetStats().mExp += 10;
//                        }
//                        if (!bullet->isPersistantBullet) {
//                            bullet->toBeDestroyed = true;
//                            break;
//                        }
//                    }
//                }
//            }
//
//            if (!bullet->toBeDestroyed) {
//                for (Boss* boss : mBossList) {
//                    if (!boss->IsAlive() || bullet->entitiesToIgnore.count(boss->GetEntity())) continue;
//
//                    float dx = bullet->mTransform.position.x - boss->GetTransform().position.x;
//                    float dz = bullet->mTransform.position.z - boss->GetTransform().position.z;
//                    if ((dx * dx + dz * dz) < thresholdSq) {
//                        if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(boss->GetEntity())) {
//                            boss->TakeDamage(bullet->mDamage);
//                            bullet->entitiesToIgnore[boss->GetEntity()] = true;
//                            if (!boss->IsAlive()) {
//                                mDestroyBossList.push_back(boss);
//                                mPlayer->GetStats().mExp += 50;
//                            }
//                            if (!bullet->isPersistantBullet) { bullet->toBeDestroyed = true; break; }
//                        }
//                    }
//                }
//            }
//        }
//
//        if (mPlayerbulletList[i]->toBeDestroyed) {
//            mDestroyBulletList.push_back(mPlayerbulletList[i]);
//            mPlayerbulletList[i] = mPlayerbulletList.back();
//            mPlayerbulletList.pop_back();
//        }
//    }
//    for (int i = mBulletList.size() - 1; i >= 0; i--) {
//        mBulletList[i]->Update();
//		Bullet* bullet = mBulletList[i];
//        if (!bullet->toBeDestroyed) {
//            if (bullet->entitiesToIgnore.count(mPlayer->mEntity)) continue;
//
//            float maxS = max(bullet->mTransform.scale.x, max(bullet->mTransform.scale.z, bullet->mTransform.scale.y));
//            float thresholdSq = (maxS + 3.0f) * (maxS + 3.0f);
//            float dx = bullet->mTransform.position.x - mPlayer->GetTransform().position.x;
//            float dz = bullet->mTransform.position.z - mPlayer->GetTransform().position.z;
//            if ((dx * dx + dz * dz) < thresholdSq) {
//                if (ecs.getComponent<ColliderComponent>(bullet->mEntity).collisionCheck(mPlayer->mEntity)) {
//                    mPlayer->takeDamage(bullet->mDamage);
//                    bullet->entitiesToIgnore[mPlayer->mEntity] = true;
//                    if (!bullet->isPersistantBullet) {
//                        bullet->toBeDestroyed = true;
//                        break;
//                    }
//                }
//            }
//        }
//
//        if (mBulletList[i]->toBeDestroyed) {
//            mDestroyBulletList.push_back(mBulletList[i]);
//            mBulletList.erase(mBulletList.begin() + i);
//        }
//    }
//}
//
//void GameManager::UpdateBar()
//{
//    ecs.getComponent<transformComponent>(healthBar).scale.x = mPlayer->GetHealth() / mPlayer->GetStats().mHealth;
//    //ecs.getComponent<transformComponent>(manaBar).scale.x = 0/*mPlayer->GetStats().mMana*/ / mPlayer->GetStats().mMana;
//}
//
//void GameManager::SpawnMob(float x, float z, int mob) {
//    EnemyMarksman* newEnemy = new EnemyMarksman();
//
//    newEnemy->Init(mPlayer->mEntity);
//    newEnemy->GetTransform() = ecs.getComponent<transformComponent>(newEnemy->mEntity);
//    //newEnemy->GetTransform().position = FLOAT3(x, 2, z);
//    mWindow->RegisterExistingMeshForEntity(newEnemy->mEntity);
//
//    mEntityMesh.insert({ newEnemy->mEntity, mFireBallMesh });
//
//    //XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newEnemy->mEntity));
//    //mWindow->Update(newEnemy->mEntity, enemyWorld);
//
//	mEnemyList.push_back(newEnemy);
//}