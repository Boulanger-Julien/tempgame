#pragma once 
#include "framework.h"
#include <DirectXMath.h>

using namespace DirectX;

class Light
{
	public:
		Light() = default;
	Light(XMFLOAT3 dir, float str, XMFLOAT4 col) : direction(dir), strength(str), color(col) {}
	XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };
	float strength = 1;
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct LightConstants
{
	XMFLOAT4 Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	XMFLOAT3 Strength = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};