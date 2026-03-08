#include "pch.h"
#include "GameManager.h"
#include "adx-core/Timer.h"
#include <DirectXMath.h>
#include <random>
#include "MeshCreator.h"
#include <string>

using namespace DirectX;

GameManager::GameManager() {
}

GameManager::GameManager(HINSTANCE hInstance)
{
    mhInstance = hInstance;

}


bool GameManager::Initialize()
{
    ecs = ECS::GetInstance();

    //Initialize window
    {
        mWindow = new Window(mhInstance);
        mWindow->Initialize(1280, 720);
    }
    // Init TextRenderer (spritesheet DDS)
    {
        mScoreTextRenderer = new TextRenderer(mWindow);
        mScoreTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    }

    InputSystem::HideCursor(true);


    //Create player
    {
        mp_player = new Player();
        ecs.getComponent<transformComponent>(mp_player->m_entity).position = FLOAT3(-2, 2, -2);
        ecs.addComponent<velocityComponent>(mp_player->m_entity, velocityComponent(1, 1, 1));
        m_playerMesh = MeshCreator::CreateBox(mWindow, mp_player->m_entity, 1.0f, 1.0f, 1.0f, XMFLOAT4(Colors::Red));
        mEntityMesh.insert({ mp_player->m_entity, m_playerMesh });
        gun = ecs.createEntity(transformComponent(0, 0, 0));
        MeshGeometry gunMesh = MeshCreator::CreateCustomMesh(mWindow, gun, "..\\..\\res\\Gun.json", 1, (XMFLOAT4)Colors::DarkRed);
        mEntityMesh.insert({ gun, gunMesh });
    }

    // Debug stuff
    {
        testEnemy = new Enemy();
        ColliderComponent tempCollider = ecs.getComponent<ColliderComponent>(testEnemy->m_entity);
        m_enemyMesh = MeshCreator::CreateCustomMesh(mWindow, testEnemy->m_entity, "..\\..\\res\\Snake.json", 1, (XMFLOAT4)Colors::Green);

        ecs.getComponent<transformComponent>(testEnemy->m_entity).position = FLOAT3(-2, 0, 4);

        mp_enemyList.push_back(testEnemy);
        mEntityMesh.insert({ testEnemy->m_entity, m_enemyMesh });

        /////

        testObstacle = new Obstacle();
        tempCollider = ecs.getComponent<ColliderComponent>(testObstacle->m_entity);
        m_obstacleMesh = MeshCreator::CreateBox(mWindow, -3, tempCollider.width, tempCollider.height, tempCollider.depth, XMFLOAT4(Colors::SaddleBrown));

        ecs.getComponent<transformComponent>(testObstacle->m_entity).position = FLOAT3(-3, 1.5f, 4);

        mp_obstacleList.push_back(testObstacle);
        mEntityMesh.insert({ testObstacle->m_entity, m_obstacleMesh });
    }

    //Generate random roads connected
    {
        Entity road1 = ecs.createEntity(transformComponent(0, 0, 0));
        road = MeshCreator::CreateRoad(mWindow, road1, 6.0f, 30.0f, 1000, (XMFLOAT4)Colors::Gray);
        mEntityMesh.insert({ road1,   road.mesh });
    }

    //Create Enemies based on the Road
    //{
    //    // Set skips to ignore most of the points (needed to not overwhelm with enemies)
    //    bool doSkip = true;
    //    bool doSkip2 = true;
    //    bool doSkip3 = false;
    //    bool doSkip4 = false;
    //    for (FLOAT2 point : road.points) {
    //        if (not doSkip) {
    //            if (not doSkip2) {
    //                if (not doSkip3) {
    //                    if (not doSkip4) {
    //                        Enemy* newEnemy = new Enemy();
    //                        ColliderComponent tempCollider = ECS::GetInstance().getComponent<ColliderComponent>(newEnemy->m_entity);
    //                        m_enemyMesh = MeshCreator::CreateBox(mWindow, -2, tempCollider.width, tempCollider.height, tempCollider.depth, XMFLOAT4(Colors::Yellow));
    //                        ECS::GetInstance().getComponent<transformComponent>(newEnemy->m_entity).position = FLOAT3(point.x, 0.5f, point.y);
    //                        mp_enemyList.push_back(newEnemy);
    //                        mEntityMesh.insert({ newEnemy->m_entity, m_enemyMesh });
    //                    }
    //                    doSkip4 = !doSkip4;
    //                }
    //                doSkip3 = !doSkip3;
    //            }
    //            doSkip2 = !doSkip2;
    //        }
    //        doSkip = !doSkip;
    //    }
    //}

    //Generate random clouds
    {
        Entity cloud = ecs.createEntity(transformComponent(0, 10, 0));
        MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\Cloud.json", 1000, (XMFLOAT4)Colors::White);
        mEntityMesh.insert({ cloud, cloudMesh });
    }

    //Generate health bar
    {
        Entity healthExtBar = ECS::GetInstance().createEntity(transformComponent(offsetHBX, offsetHBY));
        UIRenderer healthBarExtMesh(*mWindow, healthExtBar, healthBarWidth, healthBarHeight, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), L"HealthBar.dds");
        mUIMesh.insert({ healthExtBar, healthBarExtMesh.UIQuad });
        healthBar = ECS::GetInstance().createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f));
        UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
        mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
        mLifeTextRenderer = new TextRenderer(mWindow);
        mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    }

    //Setup camera
    {
        mCamera.SetPosition(0.0f, 3.0f, -10.0f);
        mCamera.SetLens(0.25f * XM_PI, mWindow->AspectRatio(), 1.0f, 1000.0f);
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

    //Create bullet mesh
    {
        m_bulletMesh = MeshCreator::CreateBall(mWindow, -1, 0.2f, 5, 5, XMFLOAT4(Colors::Teal));
    }

    mWindow->ExecuteInitCommands();
    mWindow->FlushCommandQueue();
    return true;
}

void GameManager::Update()
{
    float deltaTime = GetDeltatime();
    transformComponent& playerTrans = ecs.getComponent<transformComponent>(mp_player->m_entity);
    // generate obstacles on the road
    mTimerGenCooldown += deltaTime;
    for (auto it = road.points.rbegin(); it != road.points.rend(); ++it) {
        FLOAT2 point = *it;

        if (mTimerGenCooldown >= 3.0f) {
            // Correction de la probabilité : 20% de chance (votre commentaire disait 20 mais le code 100)
            if (rand() % 100 < 20) {

                // Calcul de la distance au carré pour éviter les appels coûteux à sqrt()
                float dx = playerTrans.position.x - point.x;
                float dz = playerTrans.position.z - point.y;
                float distSq = (dx * dx) + (dz * dz);

                // 20^2 = 400, 80^2 = 6400
                if (distSq < 6400.0f && distSq > 400.0f) {
                    Obstacle* newObstacle = new Obstacle();

                    ecs.getComponent<transformComponent>(newObstacle->m_entity).position = FLOAT3(point.x, 1.5f, point.y);

                    mp_obstacleList.push_back(newObstacle);
                    mEntityMesh.insert({ newObstacle->m_entity, m_obstacleMesh });

                    mTimerGenCooldown = 0.0f;

                    // Optionnel : break; si vous ne voulez générer qu'UN SEUL obstacle par frame
                    // break; 
                }
            }
        }
    }    // Player Shoot
    if (InputSystem::isKeyDown(VK_SPACE) && not mp_player->isCrouching) {
        if (mp_player->canShoot) {
            AddBullet(mp_player->m_entity);
            mp_player->canShoot = false;
        }
    }
    // Player Slow
    if (InputSystem::isKeyDown(VK_CONTROL)) {
        mp_player->SlowDown(true);
    }
    else {
        mp_player->SlowDown(false);
    }
    // Player Crouch
    if (InputSystem::isKeyDown('C')) {
        mp_player->isCrouching = true;
    }
    else {
        mp_player->isCrouching = false;
    }

    // Update all Bullet
    for (int i = 0; i < mp_bulletList.size(); i++) {
        mp_bulletList[i]->Update();

        if (mp_bulletList[i]->toBeDestroyed) {
            mp_destroyBulletList.push_back(mp_bulletList[i]);
            mp_bulletList.erase(mp_bulletList.begin() + i);
        }
        // COLLISION CHECKS
        else {
            //Player
            if (ecs.getComponent<ColliderComponent>(mp_bulletList[i]->m_entity).collisionCheck(mp_player->m_entity)) {
                mp_player->takeDamage();
                mp_bulletList[i]->toBeDestroyed = true;
            }
            //Enemy
            for (int j = 0; j < mp_enemyList.size(); j++) {
                if (ecs.getComponent<ColliderComponent>(mp_bulletList[i]->m_entity).collisionCheck(mp_enemyList[j]->m_entity)) {

                    mp_destroyEnemyList.push_back(mp_enemyList[j]);
                    mp_enemyList.erase(mp_enemyList.begin() + j);

                    mp_bulletList[i]->toBeDestroyed = true;
                    mp_player->m_score += 200;
                }
            }
            //Obstacle
            for (int j = mp_obstacleList.size() - 1; j >= 0; j--) {
                if (ecs.getComponent<ColliderComponent>(mp_bulletList[i]->m_entity).collisionCheck(mp_obstacleList[j]->m_entity)) {
                    mp_destroyObstacleList.push_back(mp_obstacleList[j]);
                    mp_obstacleList.erase(mp_obstacleList.begin() + j);
                    mp_bulletList[i]->toBeDestroyed = true;
                    mp_player->m_score += 50;
                    break;
                }
            }
        }
    }


    // Update Player
    mp_player->Update();

    // Update Enemies
    for (Enemy* enemy : mp_enemyList) {
        transformComponent enemyTransform = ECS::GetInstance().getComponent<transformComponent>(enemy->m_entity);
        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
        // If (canShoot) and Player is nearby (positionEnemy-positionPlayer<= or somethn idk)
        if (sqrt(pow(playerTrans.position.x - enemyTransform.position.x, 2) + pow(playerTrans.position.z - enemyTransform.position.y, 2)) < 40) {
            if (enemy->canShoot) {
                AddBullet(enemy->m_entity);
                enemy->canShoot = false;
            }
        }
    }

    // Update Obstacles
    for (int i = 0; i < mp_obstacleList.size(); i++) {
        // Player Collision Check
        if (ecs.getComponent<ColliderComponent>(mp_obstacleList[i]->m_entity).collisionCheck(mp_player->m_entity)) {
            mp_player->takeDamage();

            mp_destroyObstacleList.push_back(mp_obstacleList[i]);
            mp_obstacleList.erase(mp_obstacleList.begin() + i);
        }
    }

    // Update player position and camera position to follow the player
    {
        static size_t currentTargetIdx = 0;
        float arrivalThreshold = 0.25f;

        transformComponent& playerTrans = ecs.getComponent<transformComponent>(mp_player->m_entity);
        velocityComponent& playerVel = ecs.getComponent<velocityComponent>(mp_player->m_entity);

        FLOAT3 mov = { 0.0f, 0.0f, 0.0f };

        if (currentTargetIdx < road.points.size()) {
            Points target = road.points[currentTargetIdx];

            float dirX = target.points.x - playerTrans.position.x;
            float dirZ = target.points.y - playerTrans.position.z;

            float distance = sqrt(dirX * dirX + dirZ * dirZ);

            if (distance > arrivalThreshold) {
                mov.x = dirX / distance;
                mov.z = dirZ / distance;
            }
            else {
                currentTargetIdx++;
            }
        }

        // Update player position
        FLOAT3 pos = playerTrans.position;
        pos.x += mov.x * deltaTime * 5.0f * playerVel.velocity.x;
        pos.y += mov.y * deltaTime * 5.0f * playerVel.velocity.y;
        pos.z += mov.z * deltaTime * 5.0f * playerVel.velocity.z;

        playerTrans.position = pos;

        // Gestion de la cam�ra
        playerTrans.rotation.x -= InputSystem::GetMouseDelta().y * deltaTime;
        playerTrans.rotation.y += InputSystem::GetMouseDelta().x * deltaTime;
        playerTrans.rotation.x = std::clamp(playerTrans.rotation.x, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 1.5f);
        InputSystem::SetMousePos(400, 300);
        // Update camera position to follow the player
        {
            transformComponent& playerTrans = ecs.getComponent<transformComponent>(mp_player->m_entity);
            transformComponent camOrbit;
            camOrbit.rotation = playerTrans.rotation;

            transformSystem::RotateAround(camOrbit, playerTrans, 0.1f);

            mCamera.SetPosition(toXMFLOAT3(camOrbit.position));
            XMFLOAT3 plpos = toXMFLOAT3(playerTrans.position);
            XMFLOAT3 camPos = mCamera.Position();
            XMVECTOR camPosVect = XMLoadFloat3(&camPos);
            XMVECTOR targetVect = XMLoadFloat3(&plpos);

            mCamera.LookAt(camPosVect, targetVect);
            mWindow->SetCamera(mCamera);
        }
    }
    static int entityToRemove = -1;
    //Update gun position to be in front of the player
    {
        transformComponent& gunTrans = ecs.getComponent<transformComponent>(gun);
        transformComponent& playerTrans = ecs.getComponent<transformComponent>(mp_player->m_entity);
        float pitch = playerTrans.rotation.x;
        float yaw = playerTrans.rotation.y;

        FLOAT3 forward = {
            sin(yaw) * cos(pitch),
            -sin(pitch),
            cos(yaw) * cos(pitch)
        };

        FLOAT3 right = { cos(yaw), 0, -sin(yaw) };

        gunTrans.position = playerTrans.position - (forward * 2.5f) - (right * 0.8f) + FLOAT3(0, -0.5f, 0);

        gunTrans.rotation.y = yaw - XM_PIDIV2;
        gunTrans.rotation.z = -pitch;
    }
    ECS::GetInstance().getComponent<transformComponent>(healthBar).scale.x = mp_player->m_healthPoints / mp_player->m_maxHealthPoints;
    // Update world matrix of all entities to draw them in the correct position
    {
        for (auto it = mEntityMesh.begin(); it != mEntityMesh.end(); ++it)
        {
            int entityID = it->first;
            XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(entityID));
            mWindow->Update(entityID, entityWorld);
        }
        for (auto it = mUIMesh.begin(); it != mUIMesh.end(); ++it) {
            int entityID = it->first;
            XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(entityID));
            mWindow->UpdateUI(entityID, entityWorld);
        }
    }

    // At the end of the Update, destroy all entities unneeded
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
        MeshGeometry meshPtr = it->second;

        mWindow->Draw(meshPtr, entityID);
    }
    for (auto it = mUIMesh.begin(); it != mUIMesh.end(); ++it)
    {
        int entityID = it->first;
        MeshGeometry meshPtr = it->second;
        mWindow->DrawUI(meshPtr, entityID);
    }

    // Show text
    {
        mScoreTextRenderer->DrawTxt(mp_player->m_score > 0 ? "Score: " + std::to_string((int)mp_player->m_score) : "No Score", 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mp_player->m_healthPoints > 0 ? std::to_string((int)mp_player->m_healthPoints) + "/" + std::to_string((int)mp_player->m_maxHealthPoints) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
    }

    firstFrame = false;
    mWindow->EndFrame();
}

void GameManager::Pause()
{
    // Toggle pause when F1 is pressed
    if (InputSystem::isKeyDown(VK_F1))
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
}

/////////////////////////

void GameManager::AddBullet(Entity sender) {
    // Prevent Outdaded forward of Sender
    transformSystem::UpdateForward(ecs.getComponent<transformComponent>(sender));

    Bullet* newBullet = new Bullet();

    ecs.getComponent<transformComponent>(newBullet->m_entity) = ecs.getComponent<transformComponent>(sender);
    if (sender == mp_player->m_entity)
    {
        transformComponent& playerTrans = ecs.getComponent<transformComponent>(mp_player->m_entity);
        float pitch = playerTrans.rotation.x;
        float yaw = playerTrans.rotation.y;

        FLOAT3 forward = {
            sin(yaw) * cos(pitch),
            -sin(pitch),
            cos(yaw) * cos(pitch)
        };

        FLOAT3 right = { cos(yaw), 0, -sin(yaw) };

        ecs.getComponent<transformComponent>(newBullet->m_entity).position = playerTrans.position - (forward * 2.5f) - (right * 0.8f);
    }


    transformSystem::Move(ecs.getComponent<transformComponent>(newBullet->m_entity), 0, 0, 2);

    mEntityMesh.insert({ newBullet->m_entity,  m_bulletMesh });
    mp_bulletList.push_back(newBullet);
}

float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::Destroy() {
    if (not mp_destroyBulletList.empty()) {
        for (Bullet* bullet : mp_destroyBulletList) {
            mEntityMesh.erase(bullet->m_entity);
            delete bullet;
        }
        mp_destroyBulletList.clear();
    }

    if (not mp_destroyEnemyList.empty()) {
        for (Enemy* enemy : mp_destroyEnemyList) {
            mEntityMesh.erase(enemy->m_entity);
            delete enemy;
        }
        mp_destroyEnemyList.clear();
    }

    if (not mp_destroyObstacleList.empty()) {
        for (Obstacle* obstacle : mp_destroyObstacleList) {
            mEntityMesh.erase(obstacle->m_entity);
            delete obstacle;
        }
        mp_destroyObstacleList.clear();
    }
}