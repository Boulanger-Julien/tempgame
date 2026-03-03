#include "pch.h"
#include "GameManager.h"
#include "adx-core/Timer.h"
#include <DirectXMath.h>
#include <random>
#include "MeshCreator.h"

using namespace DirectX;

GameManager* GameManager::mp_instance = nullptr;

GameManager::GameManager() {}

GameManager::GameManager(HINSTANCE hInstance)
{
    mhInstance = hInstance;
}

GameManager GameManager::GetInstance() {
    if (mp_instance == nullptr) {
        mp_instance = new GameManager();
    }
    return *mp_instance;
}

bool GameManager::Initialize()
{
    ECS tempECS = ECS::GetInstance();
    GetInstance();
    //Initialize window
    {
        mWindow = new Window( mhInstance);
        mWindow->Initialize();
    }

    InputSystem::HideCursor(true);

    //Create player
    {
         mp_player = new Player();
        tempECS.getComponent<transformComponent>( mp_player->mp_entity).position = FLOAT3(-2,(tempECS.getComponent<transformComponent>( mp_player->mp_entity).scale.y/2.f), -2);
        tempECS.addComponent<velocityComponent>( mp_player->mp_entity, velocityComponent(1, 10, 10));
         playerMesh = MeshCreator::CreateBox(mWindow,  mp_player->mp_entity, 1.0f, 1.0f, 1.0f, XMFLOAT4(Colors::Red));
         mEntityMesh.insert({  mp_player->mp_entity,  playerMesh });
    }

    //Generate random roads connected
    {
        Entity road1 = tempECS.createEntity(transformComponent(0, 0, 0));
         road = MeshCreator::CreateRoad(mWindow, road1, 6.0f, 30.0f, 3600, (XMFLOAT4)Colors::Gray);
         mEntityMesh.insert({ road1,  road.mesh });
    }

	//Generate random clouds
    Entity cloud = tempECS.createEntity(transformComponent(0, 10, 0));
    MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\mesh_data.json", 1000, (XMFLOAT4)Colors::White);
     mEntityMesh.insert({ cloud, cloudMesh });

    //Setup camera
    {
         mCamera.SetPosition(0.0f, 3.0f, -10.0f);
         mCamera.SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);
        XMFLOAT3 pos = { 0.0f, 3.0f, -10.0f };
        XMVECTOR camPos = XMLoadFloat3(&pos);
        XMVECTOR target = XMVectorZero();
         mCamera.LookAt(camPos, target);
        mWindow->SetCamera( mCamera);
    }

    //Setup light
    {
         mLight = Light(XMFLOAT3(1.0f, -1.0f, 0.0f), 1, XMFLOAT4(1.0f, 1.f, 1.0f, 1.0f));
        mWindow->SetLight( mLight);
    }

     m_bulletMesh = MeshCreator::CreateBall(mWindow, -1, 0.2f, 5, 5, XMFLOAT4(Colors::Teal));

    mWindow->ExecuteInitCommands();
    return true;
}

void GameManager::Update(const Timer& timer)
{
    float deltaTime = GetDeltatime();

    static bool camf = false;
    static bool cDownLastFrame = false;
    // Toggle camera mode when F5 is pressed (first person / third person)
    {
        if (InputSystem::isKeyDown(VK_F5) && !camf && !cDownLastFrame)
        {
            camf = true;
            cDownLastFrame = true;
        }
        else if (InputSystem::isKeyDown(VK_F5) && camf && !cDownLastFrame)
        {
            camf = false;
            cDownLastFrame = true;
        }
        if (!InputSystem::isKeyDown(VK_F5) && cDownLastFrame)
        {
            cDownLastFrame = false;
        }
        //}

        if (InputSystem::isKeyUp('A')) {
            AddBullet();
        }

        // Update all Bullets
        for (int i = 0; i <  mp_bulletList.size(); i++) {
             mp_bulletList[i]->Update();
            if ( mp_bulletList[i]->toBeDestroyed) {
                 mp_destroyBulletList.push_back( mp_bulletList[i]);
                 mp_bulletList.erase( mp_bulletList.begin() + i);
            }
        }

        // Update player position and camera position to follow the player
        {
            static size_t currentTargetIdx = 0;
            float arrivalThreshold = 0.25f;

            transformComponent& playerTrans =  tempECS.getComponent<transformComponent>( mp_player->mp_entity);
            velocityComponent& playerVel =  tempECS.getComponent<velocityComponent>( mp_player->mp_entity);

            FLOAT3 mov = { 0.0f, 0.0f, 0.0f };

            if (currentTargetIdx <  road.points.size()) {
                FLOAT2 target =  road.points[currentTargetIdx];

                float dirX = target.x - playerTrans.position.x;
                float dirZ = target.y - playerTrans.position.z;

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
            pos.x += mov.x * timer.GetDeltatime() * 5.0f * playerVel.velocity.x;
            pos.z += mov.z * timer.GetDeltatime() * 5.0f * playerVel.velocity.y;

            playerTrans.position = pos;

            // Gestion de la cam�ra
            playerTrans.rotation.x -= InputSystem::GetMouseDelta().y * timer.GetDeltatime();
            playerTrans.rotation.y += InputSystem::GetMouseDelta().x * timer.GetDeltatime();
            playerTrans.rotation.x = std::clamp(playerTrans.rotation.x, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 1.5f);
            InputSystem::SetMousePos(400, 300);
            // Update camera position to follow the player
            {
                transformComponent& playerTrans =  tempECS.getComponent<transformComponent>( mp_player->mp_entity);
                transformComponent camOrbit;
                camOrbit.rotation = playerTrans.rotation;

                if (!camf)
                {
                    transformSystem::RotateAround(camOrbit, playerTrans, 10.0f);
                }
                else
                {
                    transformSystem::RotateAround(camOrbit, playerTrans, 0.1f);
                }

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

        // Update world matrix of all entities to draw them in the correct position
        {
            for (auto it = mEntityMesh.begin(); it != mEntityMesh.end(); ++it)
            {
                int entityID = it->first;
                XMMATRIX entityWorld = transformSystem::GetWorldMatrix(ECS::GetInstance().getComponent<transformComponent>(entityID));
                mWindow->Update(entityID, entityWorld);
            }
        }

        // At the end of the Update, destroy all ToBeDestroyed bullets
        if (not  mp_destroyBulletList.empty()) {
            DestroyBullets();
        }
    }
}

bool GameManager::Run()
{
    Timer::Reset();

    while ( msg.message != WM_QUIT)
    {
        if (PeekMessage(& msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(& msg);
            DispatchMessage(& msg);
        }
        else
        {
            Timer::Update();
            Pause();
            if (! mAppPaused)
            {
                mWindow->CalculateFrameStats();
				Update(*Timer::GetInstance());
                Draw();
            }
            else
            {
                Sleep(100);
            }
        }
    }

    return (int) msg.wParam;
}

void GameManager::Draw()
{
    mWindow->BeginFrame();

    // Draw all entities (only player for the moment)
    for (auto it =  mEntityMesh.begin(); it !=  mEntityMesh.end(); ++it)
    {
        int entityID = it->first;
        MeshGeometry meshPtr = it->second;

        mWindow->Draw(meshPtr, entityID);
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
        if ( spaceDown !=  spaceDownLastFrame)
        {
             mAppPaused = ! mAppPaused;
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

void GameManager::AddBullet() {
    Bullet* newBullet = new Bullet();

    // Init the bullet at the Player's position - then Move it so Player doesn't collide w/
    ECS::GetInstance().getComponent<transformComponent>(newBullet->m_entity).position = ECS::GetInstance().getComponent<transformComponent>( mp_player->mp_entity).position;
    ECS::GetInstance().getComponent<transformComponent>(newBullet->m_entity).forward = ECS::GetInstance().getComponent<transformComponent>( mp_player->mp_entity).forward;

     mEntityMesh.insert({ newBullet->m_entity, m_bulletMesh});
     mp_bulletList.push_back(newBullet);
}

float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::DestroyBullets() {
    for (Bullet* bullet :  mp_destroyBulletList) {
        // TODO - Remove bullet from EntityMesh & BulletList
		mEntityMesh.erase(bullet->m_entity);
        delete bullet;
    }
     mp_destroyBulletList.clear();
}