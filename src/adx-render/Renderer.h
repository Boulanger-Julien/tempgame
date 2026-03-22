#pragma once
#include "DeviceManager.h"
#include "CommandManager.h"
#include "SwapChainManager.h"
#include "DepthBufferManager.h"
#include "FenceManager.h"
#include "Camera.h"
#include "SceneData.h"
#include "RenderItem.h"
#include "RessourceManager.h"
//
#include "DescriptorManager.h"
#include "adx-render/Window.h"

class Renderer
{
public:
	void Run(HWND& _hwnd, int winW, int winH);
	void Init(HWND& _hwnd);
	void Loop();

	bool ProcessMessages();

	void Update();
	void FRender();
	void Clear();
	void SetViewport();
	void DrawScene();

	void CreateConstantBuffer(RenderItem& item);
private:
	//Window
	int mWindowWidth;
	int mWindowHeight;

	DeviceManager mDeviceManager;
	CommandManager mCommandManager;
	GraphicsPipelineManager mGraphicPipelineManager;
	SwapChainManager mSwapChainManager;
	DepthBufferManager mDepthBufferManager;
	FenceManager mFenceManager;
	Camera mCamera;
	SceneData mSceneData;

	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandle;

	DescriptorManager mDescriptorManager;
	//Draw
	RessourceManager mRessourceManager;
	std::vector<RenderItem> mRenderItems;
	//Light
	Light mLight;
	D3D12_GPU_DESCRIPTOR_HANDLE mDiffuseMapGpuHandle = {};
	//Divers
	float elapsedTime;

};

