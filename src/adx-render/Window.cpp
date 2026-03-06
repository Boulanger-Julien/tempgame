#include "pch.h"
#include "Window.h"
#include <random>

namespace {
	static std::unordered_map<int, int> sEntityToDescriptor;
	static std::unordered_map<int, int> sIndexUseTexture;
	static int sNextDescriptorIndex = 0;
	constexpr int MaxDescriptors = 1024;
}

Window::Window(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

Window::~Window()
{
}

bool Window::Initialize(int winW, int winH)
{
	std::srand(static_cast<unsigned int>(time(0)));	
	if (!D3DApp::Initialize(winW, winH))
		return false;
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	mPipelineManager.Initialize(md3dDevice.Get(), mBackBufferFormat,
		mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);
	mDescriptorManager.Initialize(md3dDevice.Get(), MaxDescriptors);
	mBoxCount = 1;
	//Draw splash screen while loading the game
	{
		mSplashScreenMesh = std::make_unique<MeshGeometry>(InitUI(L"SplashScreen.dds", 0, 1920,1080, XMFLOAT4(1, 1, 1, 1)));
		ExecuteInitCommands();
		DrawSplash();
		FlushCommandQueue();
		ResetCommandList();
	}
	return true;
}

void Window::OnResize()
{

	D3DApp::OnResize();

	float width = (float)mWindowRect.right;
	float height = (float)mWindowRect.bottom;

	if (height > 0.1f) // Sécurité
	{
		float aspect = width / height;
		Camera cam = mSceneData.GetCamera();

		// On met à jour la lentille avec le VRAI ratio
		cam.SetLens(0.25f * 3.14159f, aspect, 1.0f, 1000.0f);

		// ON RE-SAUVEGARDE DANS SCENEDATA
		mSceneData.SetCamera(cam);
	}
}

void Window::Update(int renderIndex, XMMATRIX world)
{
	ObjectConstants obj;

	if (renderIndex == 0) // Splash screen
	{
		XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(world));
		obj.UseLight = (renderIndex == 0) ? 0 : 1;
	}
	else
	{
		XMMATRIX view = mSceneData.GetViewMatrix();
		XMMATRIX proj = mSceneData.GetProjMatrix();

		XMMATRIX wvp = world * view * proj;
		XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(wvp));
	}

	XMMATRIX invWorld = XMMatrixInverse(nullptr, world);
	XMStoreFloat4x4(&obj.WorldInvTranspose, XMMatrixTranspose(invWorld));
	obj.UseTexture = sIndexUseTexture[renderIndex];

	mDescriptorManager.UpdateObjectConstants(renderIndex, obj);
}

void Window::UpdateUI(int renderIndex, XMMATRIX world)
{
	ObjectConstants obj;

	float virtualWidth = 1920.0f;
	float virtualHeight = 1080.0f;

	XMMATRIX orthoProj = XMMatrixOrthographicOffCenterLH(0, virtualWidth, virtualHeight, 0, 0.0f, 1.0f);

	XMMATRIX vvp = world * orthoProj;

	XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(vvp));
	obj.UseLight = 0;
	obj.UseTexture = sIndexUseTexture[renderIndex];

	mDescriptorManager.UpdateObjectConstants(renderIndex, obj);
}
void Window::ExecuteInitCommands()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
}

void Window::SetCamera(const Camera& cam)
{
	mSceneData.SetCamera(cam);
}

void Window::BeginFrame()
{
	mRenderContext.BeginFrame(
		mCommandList.Get(),
		mDirectCmdListAlloc.Get(),
		mPipelineManager.GetPSO(),
		CurrentBackBuffer(),
		CurrentBackBufferView(),
		DepthStencilView(),
		mScreenViewport,
		mScissorRect,
		mDescriptorManager.GetDescriptorHeap(),
		mPipelineManager.GetRootSignature()
	);
}

void Window::Draw(MeshGeometry& mGeo, int descriptorIndex)
{
	mCommandList->SetPipelineState(mPipelineManager.GetPSO());
	int textureDescriptorIndex = mGeo.Name == "TexturedGeo" ? mDescriptorManager.GetMaxDescriptors() + sIndexUseTexture[descriptorIndex] : 0;

	mRenderContext.DrawMesh(
		mCommandList.Get(),
		md3dDevice.Get(),
		mGeo,
		descriptorIndex,
		mDescriptorManager.GetMaxDescriptors(),
		textureDescriptorIndex,
		mDescriptorManager.GetDescriptorHeap()
	);
}

void Window::DrawUI(MeshGeometry& mGeo, int descriptorIndex)
{
	mCommandList->SetPipelineState(mPipelineManager.GetPSO_UI());
	int textureDescriptorIndex = mGeo.Name == "TexturedGeo" ? mDescriptorManager.GetMaxDescriptors() + sIndexUseTexture[descriptorIndex] : 0;

	mRenderContext.DrawMesh(
		mCommandList.Get(),
		md3dDevice.Get(),
		mGeo,
		descriptorIndex,
		mDescriptorManager.GetMaxDescriptors(),
		textureDescriptorIndex,
		mDescriptorManager.GetDescriptorHeap()
	);
}

void Window::EndFrame()
{
	mRenderContext.EndFrame(
		mCommandList.Get(),
		mCommandQueue.Get(),
		CurrentBackBuffer(),
		mSwapChain.Get(),
		mCurrBackBuffer,
		SwapChainBufferCount
	);

	FlushCommandQueue();
}

void Window::SetLight(const Light& light)
{
	mSceneData.SetLight(light);
	LightConstants lc = mSceneData.GetLightConstants();
	mDescriptorManager.UpdateLightConstants(lc);
}

MeshGeometry Window::BuildMesh(std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, int index, const wchar_t* filename) {

	static int sNextTextureIndex = 1;

	if (filename != nullptr)
	{
		if (sIndexUseTexture.find(index) != sIndexUseTexture.end())
		{
			OutputDebugStringA("Entity already has texture.\n");
			return BuildMesh(vertices, indices, index);
		}

		std::ifstream testFile(filename);
		if (!testFile.good())
		{
			OutputDebugStringA("The code deosnt see file at path !\n");
		}
		else
		{
			OutputDebugStringA("The file is visible by the C++.\n");
		}

		auto texture = std::make_unique<Texture>();
		texture->Filename = filename;
		texture->Name = "Text" + std::to_string(sNextTextureIndex);

		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
			mCommandList.Get(), texture->Filename.c_str(),
			texture->Resource, texture->UploadHeap));

		if (texture->Resource != nullptr)
		{
			UINT mCbvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mDescriptorManager.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

			hDescriptor.Offset(mDescriptorManager.GetMaxDescriptors() + sNextTextureIndex, mCbvDescriptorSize);

			auto desc = texture->Resource->GetDesc();

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;

			md3dDevice->CreateShaderResourceView(texture->Resource.Get(), &srvDesc, hDescriptor);

			mTextures[texture->Name] = std::move(texture);
		}

		sIndexUseTexture[index] = sNextTextureIndex;
		++sNextTextureIndex;
	}
	if (vertices.empty() || indices.empty()) {
		MeshGeometry emptyGeo;
		emptyGeo.Name = (filename != nullptr) ? "TexturedGeo" : "UnTexturedGeo";
		return emptyGeo;
	}
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

	std::unique_ptr<MeshGeometry> mGeoB = std::make_unique<MeshGeometry>();
	mGeoB->Name = (filename != nullptr) ? "TexturedGeo" : "UnTexturedGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mGeoB->VertexBufferCPU));
	CopyMemory(mGeoB->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mGeoB->IndexBufferCPU));
	CopyMemory(mGeoB->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mGeoB->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, mGeoB->VertexBufferUploader);

	mGeoB->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mGeoB->IndexBufferUploader);

	mGeoB->VertexByteStride = sizeof(Vertex);
	mGeoB->VertexBufferByteSize = vbByteSize;
	mGeoB->IndexFormat = DXGI_FORMAT_R32_UINT;
	mGeoB->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mGeoB->DrawArgs["box"] = submesh;
	if (index == 0) {
		char buf[100];
		sprintf_s(buf, "Splash Texture Index: %d\n", sIndexUseTexture[369]);
		OutputDebugStringA(buf);
	}
	return *mGeoB;
}
void Window::ResetCommandList()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}
void Window::DrawSplash()
{

		BeginFrame();
		XMMATRIX identity = XMMatrixIdentity();
		UpdateUI(0, identity);
		Draw(*mSplashScreenMesh, 0);
		EndFrame();
}

MeshGeometry Window::InitUI(const wchar_t* filename, int index, float width, float height, XMFLOAT4 color)
{
	std::vector<Vertex> vertices;
	vertices.push_back({ XMFLOAT3(0.0f, 0.0f, 0.0f), color, XMFLOAT3(0,0,-1), XMFLOAT2(0,0) }); // 0 : Haut-Gauche
	vertices.push_back({ XMFLOAT3(width, 0.0f, 0.0f), color, XMFLOAT3(0,0,-1), XMFLOAT2(1,0) }); // 1 : Haut-Droite
	vertices.push_back({ XMFLOAT3(width, height, 0.0f), color, XMFLOAT3(0,0,-1), XMFLOAT2(1,1) }); // 2 : Bas-Droite
	vertices.push_back({ XMFLOAT3(0.0f, height, 0.0f), color, XMFLOAT3(0,0,-1), XMFLOAT2(0,1) }); // 3 : Bas-Gauche
	std::vector<std::uint32_t> indices = { 0, 1, 2, 0, 2, 3 };
	std::wstring textureFilename;
	if (filename != nullptr)
	{
		textureFilename = L"..\\..\\res\\Textures\\" + std::wstring(filename);
		return MeshGeometry(BuildMesh(vertices, indices, index, textureFilename.c_str()));
	}
	else 
	{
		return MeshGeometry(BuildMesh(vertices, indices, index, filename));
	}
}
MeshGeometry Window::CreateDynamicMesh(int index, UINT maxVertices, UINT maxIndices) {
	MeshGeometry mGeo;
	mGeo.Name = "TexturedGeo";

	UINT vbByteSize = maxVertices * sizeof(Vertex);
	UINT ibByteSize = maxIndices * sizeof(uint32_t);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vbByteSize);
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mGeo.VertexBufferGPU)));

	std::vector<uint32_t> indices;
	for (UINT i = 0; i < maxVertices / 4; ++i) {
		uint32_t base = i * 4;
		indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
		indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
	}

	mGeo.IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mGeo.IndexBufferUploader);

	mGeo.VertexByteStride = sizeof(Vertex);
	mGeo.VertexBufferByteSize = vbByteSize;
	mGeo.IndexFormat = DXGI_FORMAT_R32_UINT;
	mGeo.IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = 0;
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	mGeo.DrawArgs["box"] = submesh;

	return mGeo;
}