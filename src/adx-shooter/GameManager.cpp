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
        MeshGeometry playerMesh = MeshCreator::CreateBox(mWindow, mPlayer->m_entity, 2, 2, 2, (XMFLOAT4)Colors::Navy, L"Diamond2.dds");
        mEntityMesh.insert({ mPlayer->m_entity, playerMesh });
		MeshGeometry weaponMesh = MeshCreator::CreateBox(mWindow, mPlayer->equippedWeapon->GetEntity(), 1, 0.5f, 3, (XMFLOAT4)Colors::Red, L"Diamond2.dds");
		mEntityMesh.insert({ mPlayer->equippedWeapon->GetEntity(), weaponMesh });
    }
	m_bulletMesh = MeshCreator::CreateBall(mWindow, 4, 1.0f, 10, 10, (XMFLOAT4)Colors::Blue);
    m_enemyMesh = MeshCreator::CreateBox(mWindow, 3, 2, 2, 2, (XMFLOAT4)Colors::DarkRed, L"Diamond2.dds");

    //Generate random roads connected
    {
        Entity road1 = ecs.createEntity(transformComponent(0, 0, 0));
        MeshGeometry road = MeshCreator::CreateBox(mWindow, road1, 100.0f, 1, 100, (XMFLOAT4)Colors::Gray);
        mEntityMesh.insert({ road1, road });
    }

    Entity cloud = ecs.createEntity(transformComponent(0, 10, 0));
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

    Aim();

    // Mouvement du joueur et Caméra
    transformComponent& playerTrans = mPlayer->GetTransform();
	UpdateCam(playerTrans);


    // Update de la barre de vie UI
    ecs.getComponent<transformComponent>(healthBar).scale.x = mPlayer->GetStats().mCurrentHealth / mPlayer->GetStats().mMaxHealth;

    // --- MISE À JOUR DES MATRICES DE RENDU ---
    // On met à jour les constantes de chaque entité dans le Window
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

    // Tir (Bullet instantiation)
    Shoot();
    if (InputSystem::isKeyDown(VK_RBUTTON)) // Utilisation de VK_LBUTTON pour plus de fiabilité
    {
        AddBullet(mPlayer->m_entity, mPlayer->Stats.mStrength);
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

                if (ecs.getComponent<ColliderComponent>(mPlayerbulletList[i]->m_entity).collisionCheck(enemy->mEntity)) {
                    enemy->TakeDamage(mPlayerbulletList[i]->mDamage);
                    mPlayerbulletList[i]->toBeDestroyed = true;
                    if (enemy->IsAlive() == true) {
                        mDestroyEnemyList.push_back(enemy);

                        mPlayer->Stats.mExp += 10; // Récompense d'EXP pour avoir tué un ennemi
                        break;
                    }
                }
            }
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
                mPlayer->takeDamage(mBulletList[i]->mDamage);
                mBulletList[i]->toBeDestroyed = true;
            }
        }
        if (mBulletList[i]->toBeDestroyed) {
            mDestroyBulletList.push_back(mBulletList[i]);
            mBulletList.erase(mBulletList.begin() + i);
        }
	}
    for (Enemy* enemy : mEnemyList) {
        transformComponent& enemyTransform = ECS::GetInstance().getComponent<transformComponent>(enemy->mEntity);
        enemy->Update(); //<- Maybe give PLAYER & have ENEMY turn towards PLAYER
        // If (canShoot) and Player is nearby (positionEnemy-positionPlayer<= or somethn idk)

        if (enemy->canShoot) {
            AddBullet(enemy->mEntity, enemy->GetStrength());
            enemy->canShoot = false;
        }
			//transformSystem::Move(enemyTransform, 0, 0, 0.5f);
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
		mManaTextRenderer->DrawTxt(std::to_string((int)mPlayer->GetStats().mManaPoints) + "/" + std::to_string((int)mPlayer->GetStats().mMaxManaPoints), offsetMBX + healthBarWidth * 0.06f, offsetMBY + healthBarHeight * 0.3f, 24);
        mScoreTextRenderer->DrawTxt("EXP : " + std::to_string((int)mPlayer->GetStats().mExp) , 20, 20, 24);
        mLifeTextRenderer->DrawTxt(mPlayer->GetStats().mCurrentHealth > 0 ? std::to_string((int)mPlayer->GetStats().mMaxHealth) + "/" + std::to_string((int)mPlayer->GetStats().mMaxHealth) : "Game Over", offsetHBX + healthBarWidth * 0.06f, offsetHBY + healthBarHeight * 0.3f, 24);
    }

    firstFrame = false;
    mWindow->EndFrame();
}

void GameManager::Pause()
{
	static bool spaceDown = false;
    // Toggle pause when F1 is pressed
    if (mPlayer->GetStats().mCurrentHealth == 0)
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
	if (InputSystem::isKeyDown(VK_F1) && mPlayer->GetStats().mCurrentHealth == 0)
    {
        spaceDown2 = true;
        if (spaceDown2 != spaceDownLastFrame2)
        {
            spaceDownLastFrame2 = false;
            mPlayer->GetStats().mCurrentHealth = mPlayer->GetStats().mMaxHealth;
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

void GameManager::AddBullet(Entity sender, float _damage) {
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
    const float angleStep = (2.0f * XM_PI) / bullets; // 360° / 8 en radians
    transformComponent& playerTrans = ecs.getComponent<transformComponent>(mPlayer->m_entity);

    for (int i = 0; i < bullets; ++i) {
        Bullet* newBullet = new Bullet();
        transformComponent& bulletTrans = ecs.getComponent<transformComponent>(newBullet->m_entity);

        // 1. On copie l'état du joueur
        bulletTrans = playerTrans;

        // 2. On décale la rotation Y pour chaque direction (0, 45, 90, 135...)
        bulletTrans.rotation.y = playerTrans.rotation.y + (i * angleStep);

        // --- CRUCIAL : On met à jour le vecteur 'forward' à partir de la nouvelle rotation ---
        transformSystem::UpdateForward(bulletTrans);

        // 3. Positionnement : on place la balle légèrement devant le joueur 
        // selon SA propre direction désormais unique.
        float distFromPlayer = 2.5f;
        bulletTrans.position.x += bulletTrans.forward.x * distFromPlayer;
        bulletTrans.position.y += bulletTrans.forward.y * distFromPlayer;
        bulletTrans.position.z += bulletTrans.forward.z * distFromPlayer;

        // 4. On donne une impulsion de départ (vitesse de 2)
        // Move utilise maintenant le forward mis à jour, donc chaque balle partira dans son axe.
        transformSystem::Move(bulletTrans, 0, 0, 2);

        // 5. Enregistrement graphique
        mWindow->RegisterExistingMeshForEntity(newBullet->m_entity);
        mEntityMesh.insert({ newBullet->m_entity, m_bulletMesh });

        XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(bulletTrans);
        mWindow->Update(newBullet->m_entity, bulletWorld);

        mPlayerbulletList.push_back(newBullet);
    }
}

float GameManager::GetDeltatime() {
    return Timer::GetInstance()->GetDeltatime();
}

void GameManager::Aim()
{
    if (mPlayer->aimType == AimType::Mouse)
    {
        POINT mousePos = { (LONG)InputSystem::GetMouseX(), (LONG)InputSystem::GetMouseY() };
        ScreenToClient(mWindow->MainWnd(), &mousePos);
        float finalMouseX = static_cast<float>(mousePos.x);
        float finalMouseY = static_cast<float>(mousePos.y);

        Ray ray = mCamera.GetRayFromMouse(finalMouseX, finalMouseY, mWindow->mWindowRect.right, mWindow->mWindowRect.bottom);
        mPlayer->Update(ray);
    }
    else if (mPlayer->aimType == AimType::Auto)
    {
        mPlayer->Update();
        float closestDistance = FLT_MAX;
        transformComponent& playerTrans = mPlayer->GetTransform();

        for (Enemy* enemy : mEnemyList) {
            transformComponent& enemyTrans = ecs.getComponent<transformComponent>(enemy->m_entity);
            float distance = sqrt(pow(playerTrans.position.x - enemyTrans.position.x, 2) + pow(playerTrans.position.z - enemyTrans.position.z, 2));
            if (distance < closestDistance) {
                closestDistance = distance;
            }
            else
            {
                continue;
            }
            if (distance < 70) {
                float dx = enemyTrans.position.x - playerTrans.position.x;
                float dz = enemyTrans.position.z - playerTrans.position.z;
                float angle = atan2f(dx, dz);
                playerTrans.rotation.y = angle;
            }
        }
    }
}

void GameManager::UpdateCam(transformComponent& playerTrans)
{
    mCamera.SetPosition(toXMFLOAT3(playerTrans.position + FLOAT3(30, 30, -30)));
    XMFLOAT3 playerPos = toXMFLOAT3(playerTrans.position);
    XMVECTOR targetVect = XMLoadFloat3(&playerPos);
    XMFLOAT3 camPos = mCamera.Position();
    mCamera.LookAt(XMLoadFloat3(&camPos), targetVect);
    mWindow->SetCamera(mCamera);
}

void GameManager::Shoot()
{
    static bool cDownLastFrame = false;
    static bool cDownLastFrame2 = false;

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
    if (InputSystem::isKeyDown(VK_SPACE)) {
        if (!cDownLastFrame2) {
            AddExplosionBullet(mPlayer->m_entity, 9);
            cDownLastFrame2 = true;
        }
    }
    else {
        cDownLastFrame2 = false;
    }

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
            mWindow->RemoveEntityResources(enemy->mEntity);

            // 3. On le retire du dictionnaire de rendu
            mEntityMesh.erase(enemy->mEntity);

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
    Enemy* newEnemy = new Enemy(mPlayer->m_entity);
    newEnemy->GetTransform() = ecs.getComponent<transformComponent>(newEnemy->mEntity);
    newEnemy->GetTransform().position = FLOAT3(x, 2, z);
    mWindow->RegisterExistingMeshForEntity(newEnemy->mEntity);
    mEntityMesh.insert({ newEnemy->mEntity, m_enemyMesh });
    XMMATRIX enemyWorld = transformSystem::GetWorldMatrix(ecs.getComponent<transformComponent>(newEnemy->mEntity));
    mWindow->Update(newEnemy->mEntity, enemyWorld);
	mEnemyList.push_back(newEnemy);
}