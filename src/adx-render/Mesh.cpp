#include "pch.h"
#include "Mesh.h"

void Mesh::Create(ID3D12Device* _device, const std::vector<Vertex>& _vertices, const std::vector<uint16_t>& _indices)
{
    m_indexCount = (UINT)_indices.size();

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    // Vertex buffer
    D3D12_RESOURCE_DESC vbDesc = {};
    vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vbDesc.Width = sizeof(Vertex) * _vertices.size();
    vbDesc.Height = 1;
    vbDesc.DepthOrArraySize = 1;
    vbDesc.MipLevels = 1;
    vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    vbDesc.SampleDesc.Count = 1;

    _device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    );

    void* data;
    D3D12_RANGE readRange = {};
    m_vertexBuffer->Map(0, &readRange, &data);
    memcpy(data, _vertices.data(), sizeof(Vertex) * _vertices.size());
    m_vertexBuffer->Unmap(0, nullptr);

    m_vbView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vbView.StrideInBytes = sizeof(Vertex);
    m_vbView.SizeInBytes = sizeof(Vertex) * _vertices.size();

    // Index buffer
    D3D12_RESOURCE_DESC ibDesc = vbDesc;
    ibDesc.Width = sizeof(uint32_t) * _indices.size();

    _device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &ibDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_indexBuffer)
    );

    m_indexBuffer->Map(0, &readRange, &data);
    memcpy(data, _indices.data(), sizeof(uint32_t) * _indices.size());
    m_indexBuffer->Unmap(0, nullptr);

    m_ibView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
    m_ibView.SizeInBytes = sizeof(uint32_t) * _indices.size();
    m_ibView.Format = DXGI_FORMAT_R16_UINT;
}
void Mesh::Bind(ID3D12GraphicsCommandList* _commandList) {
    _commandList->IASetVertexBuffers(0, 1, &m_vbView);
    _commandList->IASetIndexBuffer(&m_ibView);
}