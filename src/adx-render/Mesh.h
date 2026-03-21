#pragma once
#include <d3d12.h>
#include <vector>
#include <DirectXMath.h>

class Mesh
{
public:
    void Create(ID3D12Device* _device, const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices);

    void Bind(ID3D12GraphicsCommandList* _commandList);

    UINT GetIndexCount() const { return mIndexCount; }

    ID3D12Resource* GetVertexBuffer() { return mVertexBuffer; }
    ID3D12Resource* GetIndexBuffer() { return mIndexBuffer; }

    //D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return mVbView; }
    //D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() { return mIbView; }

private:
    ID3D12Resource* mVertexBuffer = nullptr;
    ID3D12Resource* mIndexBuffer = nullptr;

    D3D12_VERTEX_BUFFER_VIEW mVbView = {};
    D3D12_INDEX_BUFFER_VIEW  mIbView = {};

    UINT mIndexCount = 0;
};