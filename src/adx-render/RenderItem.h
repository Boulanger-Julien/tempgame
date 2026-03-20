#pragma once
#include "Mesh.h"
//#include "Material.h"
#include <DirectXMath.h>

using namespace DirectX;

struct RenderItem
{
    Mesh* mesh = nullptr;
    //Material* material = nullptr;

    XMFLOAT4X4 world = {};

    // Constant buffer GPU
    //ComPtr<ID3D12Resource> objectCB;
};