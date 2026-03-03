#include "pch.h"
#include "RenderContext.h"

void RenderContext::BeginFrame(
	ID3D12GraphicsCommandList* commandList,
	ID3D12CommandAllocator* commandAllocator,
	ID3D12PipelineState* pso,
	ID3D12Resource* backBuffer,
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
	const D3D12_VIEWPORT& viewport,
	const D3D12_RECT& scissorRect,
	ID3D12DescriptorHeap* descriptorHeap,
	ID3D12RootSignature* rootSignature)
{
	ThrowIfFailed(commandAllocator->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocator, pso));

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer,
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->ClearRenderTargetView(rtvHandle, Colors::CornflowerBlue, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	commandList->SetGraphicsRootSignature(rootSignature);
}

void RenderContext::EndFrame(
	ID3D12GraphicsCommandList* commandList,
	ID3D12CommandQueue* commandQueue,
	ID3D12Resource* backBuffer,
	IDXGISwapChain* swapChain,
	int& currentBackBuffer,
	int swapChainBufferCount)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);

	ThrowIfFailed(commandList->Close());

	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(swapChain->Present(0, 0));
	currentBackBuffer = (currentBackBuffer + 1) % swapChainBufferCount;
}

void RenderContext::DrawMesh(
	ID3D12GraphicsCommandList* commandList,
	ID3D12Device* device,
	MeshGeometry& mesh,
	int descriptorIndex,
	int lightDescriptorIndex,
	int textureDescriptorIndex,
	ID3D12DescriptorHeap* descriptorHeap)
{
	if (descriptorIndex < 0) descriptorIndex = 0;

	D3D12_VERTEX_BUFFER_VIEW vbv = mesh.VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW ibv = mesh.IndexBufferView();
	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Object constants
	CD3DX12_GPU_DESCRIPTOR_HANDLE objHandle(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
	objHandle.Offset(descriptorIndex, cbvDescriptorSize);
	commandList->SetGraphicsRootDescriptorTable(0, objHandle);

	// Light constants
	CD3DX12_GPU_DESCRIPTOR_HANDLE lightHandle(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
	lightHandle.Offset(lightDescriptorIndex, cbvDescriptorSize);
	commandList->SetGraphicsRootDescriptorTable(1, lightHandle);

	// Texture
	CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
	texHandle.Offset(textureDescriptorIndex, cbvDescriptorSize);
	commandList->SetGraphicsRootDescriptorTable(2, texHandle);

	commandList->DrawIndexedInstanced(mesh.DrawArgs["box"].IndexCount, 1, 0, 0, 0);
}