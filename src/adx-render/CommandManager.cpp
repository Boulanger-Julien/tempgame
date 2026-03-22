#include "pch.h"
#include "CommandManager.h"
void CommandManager::Init(ID3D12Device* _device) {
	//CommandQueue
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&mCommandQueue));
	//CommandAllocator
	_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));

	//CommandListe
	_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator, nullptr, IID_PPV_ARGS(&mCommandList));

}

void CommandManager::BeginFrame()
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);
}

void CommandManager::EndFrame()
{
	mCommandList->Close();
}
ID3D12CommandQueue* CommandManager::GetCommandQueue() {
	return mCommandQueue;
}
ID3D12CommandAllocator* CommandManager::GetCommandAllocator() {
	return mCommandAllocator;
}
ID3D12GraphicsCommandList* CommandManager::GetCommandList() {
	return mCommandList;
}