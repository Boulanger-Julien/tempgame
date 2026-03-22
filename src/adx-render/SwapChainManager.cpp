#include "pch.h"
#include "SwapChainManager.h"

void SwapChainManager::Init(HWND& _window, int _width, int _height, ID3D12Device* _device, ID3D12CommandQueue* _commandQueue) {
    IDXGIFactory4* factory = nullptr;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Width = _width;
    swap_chain_desc.BufferDesc.Height = _height;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.OutputWindow = _window;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;

    mTempSwapChain = nullptr;
    factory->CreateSwapChain(_commandQueue, &swap_chain_desc, &mTempSwapChain);

    mSwapChain = {};
    mTempSwapChain->QueryInterface(IID_PPV_ARGS(&mSwapChain));
    mTempSwapChain->Release();
    mTempSwapChain = nullptr;

    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.NumDescriptors = 2;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtv_heap_desc.NodeMask = 0;

    HRESULT hr = _device->CreateDescriptorHeap(
        &rtv_heap_desc,
        IID_PPV_ARGS(&mRtvHeap)
    );

    mRtvIncrementSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < 2; i++) {
            mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i]));

            _device->CreateRenderTargetView(mRenderTargets[i], nullptr, rtv_handle);
            rtv_handle.ptr += mRtvIncrementSize;
        }
    }
}
void SwapChainManager::BeginFrame(ID3D12GraphicsCommandList* _commandList) {
    mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = mRenderTargets[mBackBufferIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        _commandList->ResourceBarrier(1, &barrier);
    }
}
void SwapChainManager::EndFrame(ID3D12GraphicsCommandList* _commandList) {
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = mRenderTargets[mBackBufferIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        _commandList->ResourceBarrier(1, &barrier);
    }
}
IDXGISwapChain3* SwapChainManager::GetSwapChain() {
    return mSwapChain;
}

ID3D12DescriptorHeap* SwapChainManager::GetRtvHeap()
{
    return mRtvHeap;
}

ID3D12Resource* SwapChainManager::GetRenderTarget()
{
    return *mRenderTargets;
}
ID3D12Resource* SwapChainManager::GetBackBuffer() {

    return mRenderTargets[mBackBufferIndex];
}

UINT SwapChainManager::GetBackBufferIndex()
{
    return mBackBufferIndex;
}

UINT SwapChainManager::GetRtvIncrementSize()
{
    return mRtvIncrementSize;
}

