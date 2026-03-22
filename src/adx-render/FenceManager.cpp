#include "pch.h"
#include "FenceManager.h"
void FenceManager::Init(ID3D12Device* _device) {
	_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}
ID3D12Fence* FenceManager::GetFence() {
	return mFence;
}
UINT64 FenceManager::GetFenceValue() {
	return mFenceValue;
}
UINT64 FenceManager::IncrementFenceValue() {
	mFenceValue++;
	return mFenceValue;
}
HANDLE FenceManager::GetFenceEvent() {
	return mFenceEvent;
}