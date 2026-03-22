#include "pch.h"
#include "Renderer.h"

void Renderer::Run(HWND& _hwnd, int _width, int _height) {
	mWindowWidth = _width;
	mWindowHeight = _height;

	Init(_hwnd);
}
void Renderer::Init(HWND& _hwnd) {

	mDeviceManager.Init();
	mCommandManager.Init(mDeviceManager.GetDevice());
	mGraphicPipelineManager.Init(mDeviceManager.GetDevice());
	mSwapChainManager.Init(_hwnd, mWindowWidth, mWindowHeight, mDeviceManager.GetDevice(), mCommandManager.GetCommandQueue());
	mDepthBufferManager.Init(mDeviceManager.GetDevice(), mWindowWidth, mWindowHeight);
	mFenceManager.Init(mDeviceManager.GetDevice());

}