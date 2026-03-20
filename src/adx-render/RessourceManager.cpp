#include "pch.h"
#include "RessourceManager.h"
void RessourceManager::Init(ID3D12Device* _device) {

	GeometryFactory geoFacto;
	geoFacto.Init(_device);
	mCubeMesh = geoFacto.CreateGeoCube();
	//mPyramideMesh = geoFacto.CreateGeoPyramide();
	//mGridMesh = geoFacto.GenerateGrid(50, 50, 50, 50);
}

Mesh* RessourceManager::GetCubeMesh()
{
	return &mCubeMesh;
}

Mesh* RessourceManager::GetPyramideMesh()
{
	return &mPyramideMesh;
}

Mesh* RessourceManager::GetGridMesh()
{
	return &mGridMesh;
}
