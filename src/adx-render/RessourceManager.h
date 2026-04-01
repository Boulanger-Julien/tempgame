#pragma once
#include "Mesh.h"
#include "GeometryFactory.h"

class RessourceManager
{
public:
	void Init(ID3D12Device* _device);

	Mesh* GetCubeMesh();
	Mesh* GetBallMesh();
	Mesh* GetPyramideMesh();
	Mesh* GetGridMesh();

	Texture* LoadTexture(ID3D12Device* device, const std::string& name, const std::wstring& filepath);
	Texture* GetTexture(const std::string& name);

private:
	Mesh mCubeMesh;
	Mesh mPyramideMesh;
	Mesh mGridMesh;
	Mesh mBallMesh;

	Texture mCubeTexture;

	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
};

