#pragma once
#include "../adx-core/MathsUtils.h"
#include <DirectXMath.h>

using namespace DirectX;
struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = MathsUtils::Identity4x4();
    XMFLOAT4X4 World = MathsUtils::Identity4x4();
    XMFLOAT4X4 WorldInvTranspose = MathsUtils::Identity4x4();
    int UseTexture = 0; // 1 = On, 0 = Off
    UINT UseLight = 1;
    XMFLOAT3 Padding;   // Juste pour garder l'alignement propre

};