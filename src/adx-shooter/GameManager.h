#pragma once
#include <vector>
#include "Shoot\Bullet.h"
#include "Player.h"

#include "Enemy\Enemy.h"
#include "Enemy\EnemyMarksman.h"
#include "Enemy\Makhina_Boss.h"

#include "Rooms.h"
#include "Obstacle.h"
#include "MeshCreator.h"
#include "adx-render/framework.h"
#include "adx-render/Window.h"
#include "adx-render/TextRenderer.h"
#include "adx-render/UIRenderer.h"
#include "Shoot\Shoot_Pattern.h"
#include "RessourceManager.h"

class GameManager
{
public:
	static GameManager& GetInstance() {	return *instance; }
	GameManager(HINSTANCE hInstance, int winW, int winH);
	~GameManager() {}
	bool Initialize();
	void Update();
	bool Run();
	void Draw();
	void Pause();
	void SpeedDown();
	void Destroy();
	void SpawnMob(float x, float z, int mob);
	void CreateFireBall();
	void SpawnBoss(float x, float z);
	float GetDeltatime();
	void UpdateCam();
	void EnemyUpdate();
	void BulletUpdate();
	void UpdateMatrix();
	void UpdateBar();
	void GenerateRoom();
	void CheckInput();
	Camera GetCamera() { return mCamera; }

	Window* GetWindow() { return mWindow; }

	std::unordered_map<int, MeshGeometry> mEntityMesh;
	MeshGeometry mBulletMesh;
	MeshGeometry mLineBulletMesh;
	std::vector<Bullet*> mBulletList;
	std::vector<Bullet*> mPlayerbulletList;
	std::vector<Boss*> mBossList;
	std::vector<EnemyMarksman*> mEnemyList;

	/**/
	std::vector<Spell_FireBall*> mSpellListe;
	std::unordered_map<int, Mesh*> mEntityMesh2;
	float elapsedTime = 0;

	void OnInit();
	void OnUpdate();
	void CreateConstantBuffer(RenderItem& item);
	//

private:
	static GameManager* instance;//SINGELTON
	RessourceManager mRessourceManager;
	bool firstFrame = true;
	bool mAppPaused = false;
	float mSpeedDownCd = 10.0f;
	float mSpeedDownTimer = 0.0f;
	float mSpeedDownDuration = 5.0f;
	float mPlayerHealth = 100.0f;
	float mMaxPlayerHealth = 100.0f;
	float mScore = 0.0f;
	int offsetHBX = 20;
	int offsetHBY = 800;
	int offsetMBX = 20;
	int offsetMBY = 900;
	float healthBarWidth = 400;
	float healthBarHeight = 100;
	bool spaceDownLastFrame = false;
	bool spaceDown = false;
	bool spaceDownLastFrame2 = false;
	bool spaceDown2 = false;
	float timer = 0.0f;

	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;

	ECS ecs;

	Player* mPlayer;
	Entity weapon;
	Entity healthBar;
	Entity manaBar;
	Rooms currentRoom;
	Boss* newBoss;

	MeshGeometry mPlayerMesh;
	MeshGeometry mEnemyMesh;
	MeshGeometry mFireBallMesh;
	MeshGeometry MakhinaBossMesh;

	Light mLight;
	Camera mCamera;

	Road road;

	std::vector<EnemyMarksman*> mDestroyEnemyList;

	std::vector<Boss*> mDestroyBossList;

	std::vector<Bullet*> mDestroyBulletList;

	std::unordered_map<int, MeshGeometry> mUIMesh;


	TextRenderer* mScoreTextRenderer;
	TextRenderer* mLifeTextRenderer;
	TextRenderer* mManaTextRenderer;
	TextRenderer* mTimerTextRenderer;

};