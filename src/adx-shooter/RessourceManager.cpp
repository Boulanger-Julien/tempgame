void ResourceManager::Init(ID3D12Device* device)
{
    GeometryFactory factory;
    factory.Init(device);

    mMeshes["cube"] = factory.CreateGeoCube();
    mMeshes["pyramid"] = factory.CreateGeoPyramide();
    mMeshes["grid"] = factory.GenerateGrid(50, 50, 50, 50);
}