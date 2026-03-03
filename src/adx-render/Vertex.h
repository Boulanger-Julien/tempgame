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
	void FromJson(nlohmann::json j, Vertex& vertices, XMFLOAT4 color, XMFLOAT3 r) {

		// On récupère les tableaux de floats et on remplit les XMFLOAT
		auto p = j.at("pos").get<std::vector<float>>();
		vertices.Pos = XMFLOAT3(p[1] + r.x, p[2] + r.y, p[0] + r.z);

		auto c = j.at("color").get<std::vector<float>>();
		vertices.Color = color;

		auto n = j.at("normal").get<std::vector<float>>();
		vertices.Normal = XMFLOAT3(n[0], n[1], n[2]);

		auto t = j.at("texc").get<std::vector<float>>();
		vertices.TexC = XMFLOAT2(t[0], t[1]);
	}
	void FromJsonR(nlohmann::json j, Vertex& vertices, XMFLOAT4 color, bool& already_random, XMFLOAT3& randomOffset) {

		if (!already_random) {
			already_random = true;
			int sign = rand() % 2 == 0 ? -1 : 1;
			int random = rand() % 50;
			randomOffset = XMFLOAT3(random * sign, random * sign, random);
		}
		FromJson(j, vertices, color, randomOffset);
	}

};
