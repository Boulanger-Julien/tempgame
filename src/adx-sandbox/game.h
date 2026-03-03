#pragma once
#include "framework.h"
#include "Window.h"
#include "MeshCreator.h"
class Game
{
public:
	Game(HINSTANCE hInstance);
	~Game() {}
	bool Initialize();
	void Update(const Timer& timer);
	bool Run();
	void Draw();
	void Pause();

private:
	bool firstFrame = true;
	bool spaceDownLastFrame = false;
	bool spaceDown = false;
	bool mAppPaused = false;

	MSG msg = { 0 };

	HINSTANCE mhInstance;
	Window* mWindow;

	Entity player;
	Road road;

	Light mLight;
	Camera mCamera;

	std::unordered_map<int, MeshGeometry> mEntityMesh;
};