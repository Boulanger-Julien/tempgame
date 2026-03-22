#include "pch.h"
#include "DepthBufferManager.h"

void DepthBufferManager::Init(ID3D12Device* _device, int _width, int _height) {
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC depthDesc = {};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Width = _width;
    depthDesc.Height = _height;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES depthHeapProps = {};
    depthHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    _device->CreateCommittedResource(
        &depthHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&mDepthBuffer)
    );


    D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
    dsvDesc.NumDescriptors = 1;
    dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    _device->CreateDescriptorHeap(
        &dsvDesc,
        IID_PPV_ARGS(&mDsvHeap)
    );

    _device->CreateDepthStencilView(
        mDepthBuffer,
        nullptr,
        mDsvHeap->GetCPUDescriptorHandleForHeapStart()
    );
}

ID3D12Resource* DepthBufferManager::GetDepthBuffer()
{
    return mDepthBuffer;
}

ID3D12DescriptorHeap* DepthBufferManager::GetDsvHeap()
{
    return mDsvHeap;
}
