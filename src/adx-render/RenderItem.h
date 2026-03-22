#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <string>

struct RenderItem
{
    Mesh* mesh = nullptr;

    std::string name;

    DirectX::XMFLOAT4X4 world;

    ID3D12Resource* constantBuffer = nullptr;

    void CreateItem(std::string _name, Mesh* _mesh);
};