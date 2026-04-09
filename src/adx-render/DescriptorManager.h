#pragma once
#include "framework.h"

class DescriptorManager
{
public:
	DescriptorManager() = default;
	~DescriptorManager() = default;

	void Initialize(ID3D12Device* device, int maxDescriptors = 370);
	
	void UpdateObjectConstants(int renderIndex, const ObjectConstants& obj);
	void UpdateLightConstants(const LightConstants& lc);

	ID3D12DescriptorHeap* GetDescriptorHeap() const { return mCbvHeap.Get(); }
	int GetMaxDescriptors() const { return mMaxDescriptors; }

private:
	void BuildDescriptorHeaps(ID3D12Device* device);
	void BuildConstantBuffers(ID3D12Device* device);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::unique_ptr<UploadBuffer<LightConstants>> mLightCB = nullptr;
	
	int mMaxDescriptors = 370;
	int mNumDescriptors = 0;
};

