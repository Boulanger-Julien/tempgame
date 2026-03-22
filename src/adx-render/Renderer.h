#pragma once
#include "DeviceManager.h"
#include "CommandManager.h"
#include "SwapChainManager.h"
#include "DepthBufferManager.h"
#include "FenceManager.h"

class Renderer
{
public:
	void Run(HWND& _hwnd, int winW, int winH);
	void Init(HWND& _hwnd);
	void Loop();

	void Update();
	void FRender();
	void Clear();
	void SetViewPort();
	void Draw();

	void CreateConstantBuffer();
private:
	int mWindowWidth;
	int mWindowHeight;

	DeviceManager mDeviceManager;
	CommandManager mCommandManager;
	GraphicsPipelineManager mGraphicPipelineManager;
	SwapChainManager mSwapChainManager;
	DepthBufferManager mDepthBufferManager;
	FenceManager mFenceManager;
};

