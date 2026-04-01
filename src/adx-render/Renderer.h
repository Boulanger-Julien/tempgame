#pragma once
#include "DeviceManager.h"
#include "CommandManager.h"
#include "SwapChainManager.h"
#include "DepthBufferManager.h"
#include "FenceManager.h"
#include "Camera2.h"
#include "SceneData.h"
#include "RenderItem.h"
#include "RessourceManager.h"
#include "adx-shooter/GameManager.h"


class Renderer
{
public:
	static Renderer& GetInstance() { return *instance; }
	Renderer() {}
	void Run(HWND& _hwnd, int winW, int winH);
	void Init(HWND& _hwnd);
	void Loop();

	bool ProcessMessages();

	float GetDeltaTime();

	void Update(float _deltaTime);
	void FRender();
	void Clear();
	void SetViewport();
	void DrawScene();

	void CreateConstantBuffer(RenderComponent& comp);

	//
	void OnInit();
	void OnUpdate();

	RessourceManager& GetRessourceManager() { return mRessourceManager; }

private:
	static Renderer* instance;//SINGELTON

	//Window
	int mWindowWidth;
	int mWindowHeight;

	DeviceManager mDeviceManager;
	CommandManager mCommandManager;
	GraphicsPipelineManager mGraphicPipelineManager;
	SwapChainManager mSwapChainManager;
	DepthBufferManager mDepthBufferManager;
	FenceManager mFenceManager;
	Camera2 mCamera;
	SceneData mSceneData;
	GameManager mGmManager;
	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandle;

	DescriptorManager mDescriptorManager;
	//Draw
	RessourceManager mRessourceManager;
	std::vector<RenderItem> mRenderItems;
	//Light
	Light mLight;
	D3D12_GPU_DESCRIPTOR_HANDLE mDiffuseMapGpuHandle = {};
	//Divers
	std::chrono::high_resolution_clock::time_point m_previousTime;
	float elapsedTime = 0;
	//Entity
	std::vector<Entity> mEntities;

	//
	//Player* mPlayer;
};

