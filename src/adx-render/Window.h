#pragma once
#include "framework.h"
#include "../adx-core/d3dApp.h"

using namespace DirectX;

class Window : public D3DApp
{
public:
	Window(HINSTANCE hInstance);
	~Window();

	virtual bool Initialize() override;
	virtual void Draw(MeshGeometry& mGeo, int descriptorIndex) override;
	virtual void Update(int renderIndex, XMMATRIX world) override;

	void ExecuteInitCommands();
	void SetCamera(const Camera& cam);
	void SetLight(const Light& light);

	void BeginFrame();
	void EndFrame();
	MeshGeometry BuildMesh(std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, int index, const wchar_t* filename = nullptr);

private:
	virtual void OnResize() override;

	GraphicsPipelineManager mPipelineManager;
	DescriptorManager mDescriptorManager;
	SceneData mSceneData;
	RenderContext mRenderContext;

	int mBoxCount = 0;
	std::unique_ptr<MeshGeometry> mGeoB = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
};