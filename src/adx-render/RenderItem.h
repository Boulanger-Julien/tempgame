#pragma once
#include "Mesh.h"
//#include "Material.h"
#include <DirectXMath.h>

using namespace DirectX;

struct RenderItem
{
    int mEntityId;
    MeshGeometry* mesh = nullptr;

};