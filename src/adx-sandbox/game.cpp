#include "pch.h"
#include "game.h"
#include <DirectXMath.h>
#include "MeshCreator.h"

using namespace DirectX;

Game::Game(HINSTANCE hInstance, int winW, int winH)
{

    mhInstance = hInstance;
    //Initialize window
    {
        mWindow = new Window(mhInstance);
        mWindow->Initialize(winW, winH);
    }
    player = ECS::GetInstance().createEntity(transformComponent(-2, 6, -2), velocityComponent(3, 0, 3));
    mLifeTextRenderer = new TextRenderer(mWindow);
	mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
	mScoreTextRenderer = new TextRenderer(mWindow);
    mScoreTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
}

bool Game::Initialize()
{
    InputSystem::HideCursor(true);

	//Generate player
    {
        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, player, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"..\\..\\res\\Textures\\Diamond2.dds");
        mEntityMesh.insert({ player, playerMesh });
    }

    //Generate random roads connected
    {
		Entity road1 = ECS::GetInstance().createEntity(transformComponent(0, 0, 0));
        MeshGeometry road = MeshCreator::CreateBox(mWindow, road1, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
        mEntityMesh.insert({ road1, road});
    }

	Entity cloud = ECS::GetInstance().createEntity(transformComponent(0, 10, 0));
	MeshGeometry cloudMesh = MeshCreator::CreateCustomMesh(mWindow, cloud, "..\\..\\res\\Cloud.json", 1000,  (XMFLOAT4)Colors::White);
	mEntityMesh.insert({ cloud, cloudMesh });
	sun = ECS::GetInstance().createEntity(transformComponent(-200, 100, 200));
    MeshGeometry sunMesh = MeshCreator::CreateBall(mWindow, sun, 20, 30, 8, (XMFLOAT4)Colors::Yellow);
	mEntityMesh.insert({ sun, sunMesh });
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
        healthBar = ECS::GetInstance().createEntity(transformComponent(offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight*0.3f));
		UIRenderer healthBarMesh(*mWindow, healthBar, healthBarWidth * 0.9f, healthBarHeight * 0.35f, XMFLOAT4(Colors::Red));
		mUIMesh.insert({ healthBar, healthBarMesh.UIQuad });
        mLifeTextRenderer = new TextRenderer(mWindow);
		mLifeTextRenderer->Initialize(L"sheet.dds", 15, 8, 1.0f, 1.0f, 32);
    }

    mWindow->ExecuteInitCommands();
    mWindow->FlushCommandQueue();
    return true;
}

void Game::Update(const Timer& timer)
{
    static bool cDownLastFrame = false;

    // Update player position and camera position to follow the player
    {
        static size_t currentTargetIdx = 0;
        float arrivalThreshold = 0.5f;

        transformComponent& playerTrans = ECS::GetInstance().getComponent<transformComponent>(player);
        ECS::GetInstance().getComponent<transformComponent>(sun).position = FLOAT3(playerTrans.position.x - 200, 100, playerTrans.position.z + 200);
        velocityComponent& playerVel = ECS::GetInstance().getComponent<velocityComponent>(player);
        transformSystem::MoveKey(playerTrans, FLOAT3(0, -45, 0));
        FLOAT3 mov = { 0.0f, 0.0f, 0.0f };

        // Update player position
        FLOAT3 pos = playerTrans.position;
        pos.x += mov.x * timer.GetDeltatime() * 5.0f * playerVel.velocity.x * mTimerSpeed;
        pos.z += mov.z * timer.GetDeltatime() * 5.0f * playerVel.velocity.z * mTimerSpeed;

        playerTrans.position = pos;

        // Gestion de la caméra
        playerTrans.rotation.x -= InputSystem::GetMouseDelta().y * timer.GetDeltatime();
        playerTrans.rotation.y += InputSystem::GetMouseDelta().x * timer.GetDeltatime();
        playerTrans.rotation.x = std::clamp(playerTrans.rotation.x, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 1.0f);
        InputSystem::SetMousePos(400, 300);
        // Update camera position to follow the player
        {
			transformComponent camOrbit = mCamera.GetTransform();

            mCamera.SetPosition(toXMFLOAT3(playerTrans.position + FLOAT3(30,30,-30)));
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
}

bool Game::Run()
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
				SpeedDown();
                Update(*Timer::GetInstance());
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

void Game::Draw()
{
    mWindow->BeginFrame();

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
	mScore += Timer::GetInstance()->GetDeltatime() * 10.0f * mTimerSpeed;
    mScoreTextRenderer->DrawTxt(mScore > 0 ? "Score: " + std::to_string((int)mScore) : "No Score", 20, 20, 24);
	mLifeTextRenderer->DrawTxt(mPlayerHealth > 0 ? std::to_string((int)mPlayerHealth) + "/" + std::to_string((int)mMaxPlayerHealth) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);

    firstFrame = false;
    mWindow->EndFrame();

}

void Game::Pause()
{
    // Toggle pause when F1 is pressed
    if (InputSystem::isKeyDown(VK_F1))
    {
        F1Down = true;
        if (F1Down != F1DownLastFrame)
        {
            mAppPaused = !mAppPaused;
        }
        F1DownLastFrame = true;
        F1Down = false;
    }
    else
    {
        F1DownLastFrame = false;
    }
}

void Game::SpeedDown()
{
    if (InputSystem::isKeyDown(VK_SPACE))
    {
		if (mSpeedDownTimer > mSpeedDownCd)
        {
            mTimerSpeed = 0.1f;
			mSpeedDownTimer = 0.0f;
        }
    }
    if (mSpeedDownDuration > mSpeedDownTimer)
    {
        mSpeedDownTimer += Timer::GetInstance()->GetDeltatime();
    }
    else
    {
        mSpeedDownTimer += Timer::GetInstance()->GetDeltatime();
        mTimerSpeed = 1.0f;
    }
}
