#pragma once
class SwapChainManager
{
public:
	void Init(HWND& _window, int _width, int _height, ID3D12Device* _device, ID3D12CommandQueue* _commandQueue);
	void BeginFrame(ID3D12GraphicsCommandList* _commandList);
	void EndFrame(ID3D12GraphicsCommandList* _commandList);
	IDXGISwapChain3* GetSwapChain();
	ID3D12DescriptorHeap* GetRtvHeap();
	ID3D12Resource* GetRenderTarget();
	ID3D12Resource* GetBackBuffer();
	UINT GetBackBufferIndex();
	UINT GetRtvIncrementSize();
private:
	int bufferCount = 2;

	IDXGISwapChain* mTempSwapChain = nullptr;
	IDXGISwapChain3* mSwapChain = {};
	ID3D12DescriptorHeap* mRtvHeap = nullptr;
	ID3D12Resource* mRenderTargets[2];
	UINT mBackBufferIndex;
	UINT mRtvIncrementSize;
};

