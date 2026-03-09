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
    }

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
        mLifeTextRenderer = new TextRenderer(mWindow);
        mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    }

    mWindow->ExecuteInitCommands();
    mWindow->FlushCommandQueue();
    return true;
}

void GameManager::Update()
{
    float deltaTime = GetDeltatime();
    static bool cDownLastFrame = false;
	POINT mousePos = { (LONG)InputSystem::GetMouseX(), (LONG)InputSystem::GetMouseY() };
	ScreenToClient(mWindow->MainWnd(), &mousePos);
    float finalMouseX = static_cast<float>(mousePos.x);
    float finalMouseY = static_cast<float>(mousePos.y);
	Ray ray = mCamera.GetRayFromMouse(finalMouseX, finalMouseY, mWindow->mWindowRect.right, mWindow->mWindowRect.bottom);
	mPlayer->Update(ray);
    // Update player position and camera position to follow the player
    {
		transformComponent& playerTrans = mPlayer->GetTransform();
        
		transformSystem::MoveKey(playerTrans, mPlayer->Stats.mMoveSpeed, FLOAT3(0, -45, 0), deltaTime);

        // Update camera position to follow the player
        {
            transformComponent camOrbit = mCamera.GetTransform();

            mCamera.SetPosition(toXMFLOAT3(playerTrans.position + FLOAT3(30, 30, -30)));
            XMFLOAT3 plpos = toXMFLOAT3(playerTrans.position);
            XMFLOAT3 camPos = mCamera.Position();
            XMVECTOR camPosVect = XMLoadFloat3(&camPos);
            XMVECTOR targetVect = XMLoadFloat3(&plpos);

            mCamera.LookAt(camPosVect, targetVect);
            mWindow->SetCamera(mCamera);
        }
    }
    ECS::GetInstance().getComponent<transformComponent>(healthBar).scale.x = mPlayerHealth / mMaxPlayerHealth;
    static int entityToRemove = -1;
    // Update world matrix of all entities to draw them in the correct position
    {
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
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->Stats.mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->Stats.mHealthPoints > 0 ? std::to_string((int)mPlayer->Stats.mHealthPoints) + "/" + std::to_string((int)mPlayer->Stats.mMaxHealthPoints) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
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

        ecs.getComponent<transformComponent>(newBullet->m_entity).position = playerTrans.position - (forward * 2.5f) - (right * 0.8f);
    }


    transformSystem::Move(ecs.getComponent<transformComponent>(newBullet->m_entity), 0, 0, 2);

    mEntityMesh.insert({ newBullet->m_entity,  m_bulletMesh });
    mBulletList.push_back(newBullet);
}

float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::Destroy() {
    if (not mDestroyBulletList.empty()) {
        for (Bullet* bullet : mDestroyBulletList) {
            mEntityMesh.erase(bullet->m_entity);
            delete bullet;
        }
        mDestroyBulletList.clear();
    }

    if (not mDestroyEnemyList.empty()) {
        for (Enemy* enemy : mDestroyEnemyList) {
            mEntityMesh.erase(enemy->m_entity);
            delete enemy;
        }
        mDestroyEnemyList.clear();
    }

    if (not mDestroyObstacleList.empty()) {
        for (Obstacle* obstacle : mDestroyObstacleList) {
            mEntityMesh.erase(obstacle->m_entity);
            delete obstacle;
        }
        mDestroyObstacleList.clear();
    }
}