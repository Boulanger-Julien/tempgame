#pragma once
#include <d3d12.h>

class CommandManager
{
public:
	void Init(ID3D12Device* m_device);
	void BeginFrame();
	void EndFrame();
	ID3D12CommandQueue* GetCommandQueue();
	ID3D12CommandAllocator* GetCommandAllocator();
	ID3D12GraphicsCommandList* GetCommandList();

private:
	ID3D12CommandQueue* mCommandQueue = nullptr;
	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;
};

