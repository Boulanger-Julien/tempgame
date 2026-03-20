#pragma once
#include "Mesh.h"
#include "GeometryFactory.h"

class RessourceManager
{
public:
	void Init(ID3D12Device* _device);

	Mesh* GetCubeMesh();
	Mesh* GetPyramideMesh();
	Mesh* GetGridMesh();
private:
	Mesh mCubeMesh;
	Mesh mPyramideMesh;
	Mesh mGridMesh;
};

