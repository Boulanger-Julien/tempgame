#pragma once
#include "framework.h"

class RenderContext
{
public:
	RenderContext() = default;
	~RenderContext() = default;

	void BeginFrame(
		ID3D12GraphicsCommandList* commandList,
		ID3D12CommandAllocator* commandAllocator,
		ID3D12PipelineState* pso,
		ID3D12Resource* backBuffer,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
		const D3D12_VIEWPORT& viewport,
		const D3D12_RECT& scissorRect,
		ID3D12DescriptorHeap* descriptorHeap,
		ID3D12RootSignature* rootSignature);

	void EndFrame(
		ID3D12GraphicsCommandList* commandList,
		ID3D12CommandQueue* commandQueue,
		ID3D12Resource* backBuffer,
		IDXGISwapChain* swapChain,
		int& currentBackBuffer,
		int swapChainBufferCount);

	void DrawMesh(
		ID3D12GraphicsCommandList* commandList,
		ID3D12Device* device,
		MeshGeometry& mesh,
		int descriptorIndex,
		int lightDescriptorIndex,
		int textureDescriptorIndex,
		ID3D12DescriptorHeap* descriptorHeap);
};