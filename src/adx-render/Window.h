#pragma once
#include "framework.h"
#include "../adx-core/d3dApp.h"
#include "Mesh.h"
#include "RenderItem.h"

using namespace DirectX;

class Window : public D3DApp
{
public:
	Window(HINSTANCE hInstance);
	~Window();

	virtual bool Initialize(int winW, int winH) override;
	
	virtual void Draw(MeshGeometry& mGeo, int descriptorIndex) override;
	void DrawUI(MeshGeometry& mGeo, int descriptorIndex);
	virtual void Update(int renderIndex, XMMATRIX world) override;
	void UpdateUI(int renderIndex, XMMATRIX world);

	void ExecuteInitCommands();
	void SetCamera(const Camera& cam);
	void SetLight(const Light& light);

	void BeginFrame();
	void EndFrame();
	MeshGeometry BuildMesh(std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, int index, const wchar_t* filename = nullptr);

	void ResetCommandList();
	MeshGeometry InitUI(const wchar_t* filename, int index, float width, float height, XMFLOAT4 color);

	MeshGeometry CreateDynamicMesh(int index, UINT maxVertices, UINT maxIndices);
	void RegisterExistingMeshForEntity(int entityID);
	void RemoveEntityResources(int entityID);

	/**/
	void AddRenderItem(Mesh* mesh,/* Material* mat,*/ XMFLOAT4X4 world);
	void DrawRenderItems(RenderItem _renderItem);
	ID3D12Device* GetDevice() const;
	std::vector<RenderItem> mRenderItems;

	SceneData mSceneData;
	//
private:


	virtual void OnResize() override;
	void DrawSplash();



	GraphicsPipelineManager mPipelineManager;
	DescriptorManager mDescriptorManager;
	RenderContext mRenderContext;

	int mBoxCount = 0;
	std::unique_ptr<MeshGeometry> mGeoB = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unique_ptr<Texture> mSplashScreenTexture;
	std::unique_ptr<MeshGeometry> mSplashScreenMesh;
	bool mIsSplashScreenReady = false;

};