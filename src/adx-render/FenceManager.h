#pragma once
class FenceManager
{
public:
	void Init(ID3D12Device* _device);

	ID3D12Fence* GetFence();
	UINT64 GetFenceValue();
	UINT64 IncrementFenceValue();
	HANDLE GetFenceEvent();
private:
	ID3D12Fence* mFence = nullptr;
	UINT64 mFenceValue = 0;
	HANDLE mFenceEvent;
};

