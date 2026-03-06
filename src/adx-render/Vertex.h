#pragma once
#include <DirectXMath.h>
#include "json.hpp"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	void FromJson(nlohmann::json j, XMFLOAT4 color) {
		auto p = j.at("pos").get<std::vector<float>>();
		Pos = XMFLOAT3(p[1], p[2], p[0]);

		Color = color;

		Normal = XMFLOAT3(0,1,0);

		TexC = XMFLOAT2(0,0);
	}
};
