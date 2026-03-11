#include "pch.h"
#include "GameManager.h"
#include "adx-core/Timer.h"
#include <DirectXMath.h>
#include <random>
#include "MeshCreator.h"
#include <string>

using namespace DirectX;

GameManager::GameManager(HINSTANCE hInstance, int winW, int winH)
{
    mhInstance = hInstance;
    //Initialize window
    {
        mWindow = new Window(mhInstance);
        mWindow->Initialize(winW, winH);
    }
	mPlayer = new Player();
    mLifeTextRenderer = new TextRenderer(mWindow);
    mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mManaTextRenderer = new TextRenderer(mWindow);
    mManaTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    mScoreTextRenderer = new TextRenderer(mWindow);
    mScoreTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
}


bool GameManager::Initialize()
{
    ecs = ECS::GetInstance();

    //Generate player
    {
        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, mPlayer->m_entity, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"..\\..\\res\\Textures\\Diamond2.dds");
        mEntityMesh.insert({ mPlayer->m_entity, playerMesh });
		MeshGeometry weaponMesh = MeshCreator::CreateBox(mWindow, mPlayer->equippedWeapon->GetEntity(), 1, 0.5f, 3, (XMFLOAT4)Colors::Red, L"..\\..\\res\\Textures\\Diamond2.dds");
		mEntityMesh.insert({ mPlayer->equippedWeapon->GetEntity(), weaponMesh });
    }
	m_bulletMesh = MeshCreator::CreateBall(mWindow, 4, 1.0f, 10, 10, (XMFLOAT4)Colors::Blue);
    m_enemyMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"..\\..\\res\\Textures\\Diamond2.dds");

    //Generate random roads connected
    {
        Entity road1 = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
        MeshGeometry road = MeshCreator::CreateBox(mWindow, road1, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
        mEntityMesh.insert({ road1, road });
    }

    Entity cloud = ECS::GetInstance().createEntity(transformComponent(0, 10, 0));
    MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\Cloud.json", 1000, (XMFLOAT4)Colors::White);
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
        Entity healthExtBar = ECS::GetInstance().createEntity(transformComponent(offsetHBX, offsetHBY));
        UIRenderer healthBarExtMesh(*mWindow, healthExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
        mUIMesh.insert({ healthExtBar, healthBarExtMesh.UIQuad });
        healthBar = ECS::GetInstance().createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f));
        UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
        mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
    }
    {
        Entity manaExtBar = ECS::GetInstance().createEntity(transformComponent(offsetMBX, offsetMBY));
        UIRenderer healthBarExtMesh(*mWindow, manaExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
        mUIMesh.insert({ manaExtBar, healthBarExtMesh.UIQuad });
        manaBar = ECS::GetInstance().createEntity(transformComponent(offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f));
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
    static bool cDownLastFrame = false;

    // Input & Player Update
    POINT mousePos = { (LONG)InputSystem::GetMouseX(), (LONG)InputSystem::GetMouseY() };
    ScreenToClient(mWindow->MainWnd(), &mousePos);
    float finalMouseX = static_cast<float>(mousePos.x);
    float finalMouseY = static_cast<float>(mousePos.y);

    Ray ray = mCamera.GetRayFromMouse(finalMouseX, finalMouseY, mWindow->mWindowRect.right, mWindow->mWindowRect.bottom);
    mPlayer->Update(ray);

    // Mouvement du joueur et Caméra
    transformComponent& playerTrans = mPlayer->GetTransform();
    transformSystem::MoveKey(playerTrans, mPlayer->Stats.mMoveSpeed, FLOAT3(0, -45, 0), deltaTime);

    mCamera.SetPosition(toXMFLOAT3(playerTrans.position + FLOAT3(30, 30, -30)));
	XMFLOAT3 playerPos = toXMFLOAT3(playerTrans.position);
    XMVECTOR targetVect = XMLoadFloat3(&playerPos);
	XMFLOAT3 camPos = mCamera.Position();
    mCamera.LookAt(XMLoadFloat3(&camPos), targetVect);
    mWindow->SetCamera(mCamera);

    // Update de la barre de vie UI
    ECS::GetInstance().getComponent<transformComponent>(healthBar).scale.x = mPlayer->Stats.mHealthPoints / mPlayer->Stats.mMaxHealthPoints;

    // --- MISE À JOUR DES MATRICES DE RENDU ---
    // On met à jour les constantes de chaque entité dans le Window
    for (auto it = mEntityMesh.begin(); it != mEntityMesh.end(); ++it)
    {
        int entityID = it->first;
        XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(entityID));
        mWindow->Update(entityID, entityWorld);
    }
    for (auto it = mUIMesh.begin(); it != mUIMesh.end(); ++it)
    {
        int entityID = it->first;
        XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(entityID));
        mWindow->UpdateUI(entityID, entityWorld);
    }

    // Tir (Bullet instantiation)
    if (InputSystem::isKeyDown(VK_LBUTTON)) // Utilisation de VK_LBUTTON pour plus de fiabilité
    {
        if (!cDownLastFrame) {
            AddBullet(mPlayer->m_entity);
            cDownLastFrame = true;
        }
    }
    else {
        cDownLastFrame = false;
    }
    if (InputSystem::isKeyDown(VK_RBUTTON)) // Utilisation de VK_LBUTTON pour plus de fiabilité
    {
        AddBullet(mPlayer->m_entity);
    }
    if (InputSystem::isKeyDown('C')) // Utilisation de VK_LBUTTON pour plus de fiabilité
    {
		SpawnMob(rand() % 100 - 50, rand() % 100 - 50, 0);
    }

    // --- BOUCLE DES BULLETS ET COLLISIONS ---
    for (int i = mPlayerbulletList.size() - 1; i >= 0; i--) {
        mPlayerbulletList[i]->Update();

        // Vérification de collision avec les ennemis (si la balle n'est pas déjà marquée)
        if (!mPlayerbulletList[i]->toBeDestroyed) {
            for (Enemy* enemy : mEnemyList) {
                // TRÈS IMPORTANT : On vérifie s'il n'est pas déjà mort
                if (enemy->isDead) continue;

                if (ecs.getComponent<ColliderComponent>(mPlayerbulletList[i]->m_entity).collisionCheck(enemy->m_entity)) {
                    enemy->isDead = true; // On le marque immédiatement
                    mDestroyEnemyList.push_back(enemy);
                    mPlayerbulletList[i]->toBeDestroyed = true;
                    break;
                }
            }
        
            //{
            //    if (ecs.getComponent<ColliderComponent>(mBulletList[i]->m_entity).collisionCheck(mPlayer->m_entity)) {
            //        mPlayer->takeDamage(2);
            //        mBulletList[i]->toBeDestroyed = true;
            //    }
            //}
        }

        // Si la balle doit être détruite (sortie d'écran ou collision)
        if (mPlayerbulletList[i]->toBeDestroyed) {
            mDestroyBulletList.push_back(mPlayerbulletList[i]);
            mPlayerbulletList.erase(mPlayerbulletList.begin() + i);
        }
    }
    for (int i = mBulletList.size() - 1; i >= 0; i--) {
        mBulletList[i]->Update();
        if (!mBulletList[i]->toBeDestroyed) {
            if (ecs.getComponent<ColliderComponent>(mBulletList[i]->m_entity).collisionCheck(mPlayer->m_entity)) {
                mPlayer->takeDamage(20);
                mBulletList[i]->toBeDestroyed = true;
            }
        }
        if (mBulletList[i]->toBeDestroyed) {
            mDestroyBulletList.push_back(mBulletList[i]);
            mBulletList.erase(mBulletList.begin() + i);
        }
	}
    for (Enemy* enemy : mEnemyList) {
        transformComponent& enemyTransform = ECS::GetInstance().getComponent<transformComponent>(enemy->m_entity);
        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
		enemy->LookAt(mPlayer->m_entity);
        // If (canShoot) and Player is nearby (positionEnemy-positionPlayer<= or somethn idk)
        if (sqrt(pow(playerTrans.position.x - enemyTransform.position.x, 2) + pow(playerTrans.position.z - enemyTransform.position.y, 2)) < 40) {
            if (enemy->canShoot) {
                AddBullet(enemy->m_entity);
                enemy->canShoot = false;
            }
			//transformSystem::Move(enemyTransform, 0, 0, 0.5f);
        }
    }
    // Nettoyage final des entités supprimées ce frame
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

    // Show text
    {
		mManaTextRenderer->DrawTxt(std::to_string((int)mPlayer->Stats.mManaPoints) + "/" + std::to_string((int)mPlayer->Stats.mMaxManaPoints), offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f, 24);
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->Stats.mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->Stats.mHealthPoints > 0 ? std::to_string((int)mPlayer->Stats.mHealthPoints) + "/" + std::to_string((int)mPlayer->Stats.mMaxHealthPoints) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
    }

    firstFrame = false;
    mWindow->EndFrame();
}

void GameManager::Pause()
{
    // Toggle pause when F1 is pressed
    if (mPlayer->Stats.mHealthPoints == 0)
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
	if (InputSystem::isKeyDown(VK_F1) && mPlayer->Stats.mHealthPoints == 0)
    {
        spaceDown2 = true;
        if (spaceDown2 != spaceDownLastFrame2)
        {
            spaceDownLastFrame2 = false;
            mPlayer->Stats.mHealthPoints = mPlayer->Stats.mMaxHealthPoints;
            mAppPaused = !mAppPaused;
            for (Bullet* bullet : mBulletList) {
                bullet->toBeDestroyed = true;
			}
			Destroy();
        }
        spaceDownLastFrame2 = true;
        spaceDown2 = false;
    }
    else
    {
        spaceDownLastFrame2 = false;
    }
}

/////////////////////////

void GameManager::AddBullet(Entity sender) {
    // Prevent Outdaded forward of Sender
    transformSystem::UpdateForward(ecs.getComponent<transformComponent>(sender));

    Bullet* newBullet = new Bullet();

    ecs.getComponent<transformComponent>(newBullet->m_entity) = ecs.getComponent<transformComponent>(sender);
    if (sender == mPlayer->m_entity)
    {
        transformComponent& playerTrans = ecs.getComponent<transformComponent>(mPlayer->m_entity);
        float pitch = playerTrans.rotation.x;
        float yaw = playerTrans.rotation.y;

        FLOAT3 forward = {
            sin(yaw) * cos(pitch),
            -sin(pitch),
            cos(yaw) * cos(pitch)
        };

        FLOAT3 right = { cos(yaw), 0, -sin(yaw) };

        ecs.getComponent<transformComponent>(newBullet->m_entity).position = playerTrans.position + (forward * 2.5f);
    }
	transformComponent& bulletTrans = ecs.getComponent<transformComponent>(newBullet->m_entity);
	bulletTrans.forward = bulletTrans.forward * -1;

    transformSystem::Move(ecs.getComponent<transformComponent>(newBullet->m_entity), 0, 0, 2);

    mWindow->RegisterExistingMeshForEntity(newBullet->m_entity);
    mEntityMesh.insert({ newBullet->m_entity, m_bulletMesh });
    XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newBullet->m_entity));
    mWindow->Update(newBullet->m_entity, bulletWorld);
    if (sender == mPlayer->m_entity) {
        mPlayerbulletList.push_back(newBullet);
	}
    else
    {
        mBulletList.push_back(newBullet);
    }
}
float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::Destroy() {
    // NETTOYAGE DES BULLETS
    if (!mDestroyBulletList.empty()) {
        for (Bullet* bullet : mDestroyBulletList) {
            // 1. Libérer les ressources DirectX (Slots de descripteurs)
            mWindow->RemoveEntityResources(bullet->m_entity);

            // 2. Retirer du système de rendu
            mEntityMesh.erase(bullet->m_entity);

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
            mWindow->RemoveEntityResources(enemy->m_entity);

            // 3. On le retire du dictionnaire de rendu
            mEntityMesh.erase(enemy->m_entity);

            // 4. Supprimer l'objet C++
            delete enemy;
        }
        mDestroyEnemyList.clear();
    }

    // NETTOYAGE DES OBSTACLES
    if (!mDestroyObstacleList.empty()) {
        for (Obstacle* obstacle : mDestroyObstacleList) {
            mWindow->RemoveEntityResources(obstacle->m_entity);
            mEntityMesh.erase(obstacle->m_entity);
            delete obstacle;
        }
        mDestroyObstacleList.clear();
    }
}

void GameManager::SpawnMob(float x, float z, int mob) {
    Enemy* newEnemy = new Enemy();
    newEnemy->GetTransform() = ECS::GetInstance().getComponent<transformComponent>(newEnemy->m_entity);
    newEnemy->GetTransform().position = FLOAT3(x, 2, z);
    mWindow->RegisterExistingMeshForEntity(newEnemy->m_entity);
    mEntityMesh.insert({ newEnemy->m_entity, m_enemyMesh });
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(newEnemy->m_entity));
    mWindow->Update(newEnemy->m_entity, enemyWorld);
	mEnemyList.push_back(newEnemy);
}