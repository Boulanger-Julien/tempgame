#include "pch.h"
#include "DescriptorManager.h"

void DescriptorManager::Initialize(ID3D12Device* device, int maxDescriptors)
{
	mMaxDescriptors = maxDescriptors;
	BuildDescriptorHeaps(device);
	BuildConstantBuffers(device);
}

void DescriptorManager::BuildDescriptorHeaps(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	mNumDescriptors = mMaxDescriptors + 1; // +1 pour la lumière

	cbvHeapDesc.NumDescriptors = mNumDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));
}

void DescriptorManager::BuildConstantBuffers(ID3D12Device* device)
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, mNumDescriptors, true);
	mLightCB = std::make_unique<UploadBuffer<LightConstants>>(device, 1, true);
	
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT lightCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(LightConstants));
	UINT cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < mMaxDescriptors; ++i)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
		cbAddress += i * objCBByteSize;

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(i, cbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = objCBByteSize;
		device->CreateConstantBufferView(&cbvDesc, handle);
	}

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mLightCB->Resource()->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(mMaxDescriptors, cbvDescriptorSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = lightCBByteSize;
	device->CreateConstantBufferView(&cbvDesc, handle);
}

void DescriptorManager::UpdateObjectConstants(int renderIndex, const ObjectConstants& obj)
{
	mObjectCB->CopyData(renderIndex, obj);
}

void DescriptorManager::UpdateLightConstants(const LightConstants& lc)
{
	mLightCB->CopyData(0, lc);
}
