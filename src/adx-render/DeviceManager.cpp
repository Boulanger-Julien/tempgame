#include "pch.h"
#include "DeviceManager.h"

void DeviceManager::Init()
{
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
}

ID3D12Device* DeviceManager::GetDevice()
{
	return mDevice;
}
