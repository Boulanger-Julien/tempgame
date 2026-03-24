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
Texture* RessourceManager::LoadTexture(ID3D12Device* device, const std::string& name, const std::wstring& filepath)
{
    // Vérifie si la texture existe déjà
    auto it = mTextures.find(name);
    if (it != mTextures.end())
        return it->second.get();

    // Crée et charge la texture
    auto tex = std::make_unique<Texture>();
    tex->Name = name;
    tex->Filename = filepath;

    // Utilise DirectX::CreateDDSTextureFromFile12 ou WIC
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
        device,
        nullptr,            // pas de command list pour l'instant
        tex->Filename.c_str(),
        tex->Resource,
        tex->UploadHeap
    ));

    Texture* ptr = tex.get();             // pointeur brut pour retourner
    mTextures[name] = std::move(tex);     // stocke dans le manager

    return ptr;
}
Texture* RessourceManager::GetTexture(const std::string& name)
{
    auto it = mTextures.find(name);
    if (it != mTextures.end())
        return it->second.get();
    return nullptr;
}