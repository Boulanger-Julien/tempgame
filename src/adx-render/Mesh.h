#pragma once
#include <d3d12.h>
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"

class Mesh
{
public:
    void Create(ID3D12Device* _device, const std::vector<Vertex>& _vertices, const std::vector<uint16_t>& _indices);

    void Bind(ID3D12GraphicsCommandList* _commandList);

    UINT GetIndexCount() const { return m_indexCount; }

private:
    ID3D12Resource* m_vertexBuffer = nullptr;
    ID3D12Resource* m_indexBuffer = nullptr;

    D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
    D3D12_INDEX_BUFFER_VIEW  m_ibView = {};

    UINT m_indexCount = 0;
};