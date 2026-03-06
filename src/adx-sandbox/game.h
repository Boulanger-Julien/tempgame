#pragma once
#include "framework.h"
#include "Window.h"
#include "MeshCreator.h"

class Game
{
public:
	Game(HINSTANCE hInstance, int winW, int winH);
	~Game() {}
	bool Initialize();
	void Update(const Timer& timer);
	bool Run();
	void Draw();
	void Pause();
	void SpeedDown();

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
	Entity gun;
	Road road;
	Light mLight;
	Camera mCamera;
	TextRenderer* mScoreTextRenderer;
	TextRenderer* mLifeTextRenderer;

	float mTimerSpeed = 1.0f;

	std::unordered_map<int, MeshGeometry> mEntityMesh;
	std::unordered_map<int, MeshGeometry> mUIMesh;

};