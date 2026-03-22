#pragma once
#include <d3d12.h>

class DeviceManager
{
public:
	void Init();
	ID3D12Device* GetDevice();
private:
	ID3D12Device* mDevice;
};

