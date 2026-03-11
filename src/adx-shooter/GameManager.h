#pragma once
#include <vector>
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "Obstacle.h"
#include "MeshCreator.h"
#include "adx-render/framework.h"
#include "adx-render/Window.h"
#include "adx-render/TextRenderer.h"
#include "adx-render/UIRenderer.h"

class GameManager
{
public:
	GameManager(HINSTANCE hInstance, int winW, int winH);
	~GameManager() {}
	bool Initialize();
	void Update();
	bool Run();
	void Draw();
	void Pause();
	void SpeedDown();
	void AddBullet(Entity sender);
	void Destroy();
	void SpawnMob(float x, float z, int mob);
	float GetDeltatime();


private:
	bool firstFrame = true;
	bool F1DownLastFrame = false;
	bool F1Down = false;
	bool mAppPaused = false;
	float mSpeedDownCd = 10.0f;
	float mSpeedDownTimer = 0.0f;
	float mSpeedDownDuration = 5.0f;
	float mPlayerHealth = 100.0f;
	float mMaxPlayerHealth = 100.0f;
	float mScore = 0.0f;
	int offsetHBX = 20;
	int offsetHBY = 800;
	float healthBarWidth = 400;
	float healthBarHeight = 100;
	bool spaceDownLastFrame = false;
	bool spaceDown = false;
	bool spaceDownLastFrame2 = false;
	bool spaceDown2 = false;


	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;

	ECS ecs;

	Player* mPlayer;
	Entity weapon;
	Entity healthBar;

	MeshGeometry m_obstacleMesh;
	MeshGeometry m_playerMesh;
	MeshGeometry m_enemyMesh;
	MeshGeometry m_bulletMesh;

	Light mLight;
	Camera mCamera;

	Road road;

	std::vector<Obstacle*> mObstacleList;
	std::vector<Obstacle*> mDestroyObstacleList;

	std::vector<Enemy*> mEnemyList;
	std::vector<Enemy*> mDestroyEnemyList;

	std::vector<Bullet*> mBulletList;
	std::vector<Bullet*> mPlayerbulletList;
	std::vector<Bullet*> mDestroyBulletList;

	std::unordered_map<int, MeshGeometry> mUIMesh;
	std::unordered_map<int, MeshGeometry> mEntityMesh;

	TextRenderer* mScoreTextRenderer;
	TextRenderer* mLifeTextRenderer;

};