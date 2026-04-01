#pragma once
#include "adx-render/Mesh.h"

struct RenderComponent
{
    Mesh* mesh = nullptr;

	ID3D12Resource* constantBuffer = nullptr;
    //Texture* texture = nullptr;
};