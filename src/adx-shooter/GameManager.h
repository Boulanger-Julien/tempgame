#pragma once
#include <vector>
#include "Bullet.h"
#include "Player.h"
#include "MeshCreator.h"
#include "adx-render/framework.h"
#include "adx-render/Window.h"

class GameManager
{
private:
	static GameManager* mp_instance;

	bool firstFrame = true;
	bool spaceDownLastFrame = false;
	bool spaceDown = false;
	bool mAppPaused = false;

	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;

	ECS tempECS;

	Player* mp_player;

	MeshGeometry playerMesh;
	MeshGeometry m_bulletMesh;

	Light mLight;
	Camera mCamera;

	Road road;

	std::vector<Bullet*> mp_bulletList;
	std::vector<Bullet*> mp_destroyBulletList;

	std::unordered_map<int, MeshGeometry> mEntityMesh;		// Stores Entities & their Meshs

public:
	GameManager();
	GameManager(HINSTANCE hInstance);

	static GameManager GetInstance();

	bool Initialize();
	void Update(const Timer& timer);
	bool Run();
	void Draw();
	void Pause();

	float GetDeltatime();
	void AddBullet();
	void DestroyBullets();
};

