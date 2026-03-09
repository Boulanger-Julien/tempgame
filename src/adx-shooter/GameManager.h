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
private:
	bool firstFrame = true;
	bool spaceDownLastFrame = false;
	bool spaceDown = false;
	bool mAppPaused = false;

	int offsetHBX = 20;
	int offsetHBY = 800;
	float healthBarWidth = 400;
	float healthBarHeight = 100;
	float mTimerGenCooldown = 3.0f;


	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;

	ECS ecs;

	Player* mp_player;
	Enemy* testEnemy; // Just for debug 
	Obstacle* testObstacle; // Ditto
	Obstacle* newObstacle; // Used to generate new obstacles on the road
	Entity gun;
	Entity healthBar;

	MeshGeometry m_obstacleMesh;
	MeshGeometry m_playerMesh;
	MeshGeometry m_enemyMesh;
	MeshGeometry m_bulletMesh;

	Light mLight;
	Camera mCamera;

	Road road;

	std::vector<Obstacle*> mp_obstacleList;
	std::vector<Obstacle*> mp_destroyObstacleList;

	std::vector<Enemy*> mp_enemyList;
	std::vector<Enemy*> mp_destroyEnemyList;

	std::vector<Bullet*> mp_bulletList;
	std::vector<Bullet*> mp_PlayerbulletList;
	std::vector<Bullet*> mp_destroyBulletList;

	std::unordered_map<int, MeshGeometry> mUIMesh;
	std::unordered_map<int, MeshGeometry> mEntityMesh;		// Stores Entities & their Meshs

	TextRenderer* mScoreTextRenderer;
	TextRenderer* mLifeTextRenderer;

public:
	GameManager();
	GameManager(HINSTANCE hInstance);

	bool Initialize();
	void Update();
	bool Run();
	void Draw();
	void Pause();

	float GetDeltatime();
	void AddBullet(Entity sender);
	void Destroy();
};

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


	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;
	Entity healthBar;
	Entity player;
	Entity sun;
	Light mLight;
	Camera mCamera;
	TextRenderer* mScoreTextRenderer;
	TextRenderer* mLifeTextRenderer;

	float mTimerSpeed = 1.0f;

	std::unordered_map<int, MeshGeometry> mEntityMesh;
	std::unordered_map<int, MeshGeometry> mUIMesh;

};