#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <array>

class GeometryFactory
{
public:
	void Init(ID3D12Device* _device);

	void ComputeNormals(std::vector<Vertex>& verts, const std::vector<std::uint32_t>& inds);

	Mesh CreateGeoCube();
	//Mesh CreateGeoPyramide();
	//Mesh GenerateGrid(float _width, float _depth, uint32_t col, uint32_t row);

private:
	ID3D12Device* mDevice = nullptr;
};