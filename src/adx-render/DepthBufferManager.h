#pragma once
class DepthBufferManager
{
public:
	void Init(ID3D12Device* _device, int _width, int _height);

	ID3D12Resource* GetDepthBuffer();
	ID3D12DescriptorHeap* GetDsvHeap();
private:
	ID3D12Resource* mDepthBuffer = nullptr;
	ID3D12DescriptorHeap* mDsvHeap = nullptr;
};

