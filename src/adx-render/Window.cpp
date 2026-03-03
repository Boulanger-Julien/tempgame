#include "pch.h"
#include "Window.h"
#include <random>

namespace {
	static std::unordered_map<int, int> sEntityToDescriptor;
	static std::unordered_map<int, int> sIndexUseTexture;
	static int sNextDescriptorIndex = 0;
	constexpr int MaxDescriptors = 370;
}

Window::Window(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

Window::~Window()
{
}

bool Window::Initialize()
{
	std::srand(time(0));
	if (!D3DApp::Initialize())
		return false;
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	mPipelineManager.Initialize(md3dDevice.Get(), mBackBufferFormat,
		mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);
	mDescriptorManager.Initialize(md3dDevice.Get(), 370);
	mBoxCount = 1;

	// Execute the initialization commands.
	//ThrowIfFailed(mCommandList->Close());
	//ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	//mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//OnResize();
	// Wait until initialization is complete.
	FlushCommandQueue();
	return true;
}

void Window::OnResize()
{
	D3DApp::OnResize();

	float aspect = mScreenViewport.Width / mScreenViewport.Height;
	Camera cam = mSceneData.GetCamera();
	cam.SetLens(0.25f * XM_PI, aspect, 1.0f, 1000.0f);
	mSceneData.SetCamera(cam);
}

void Window::Update(int renderIndex, XMMATRIX world)
{
	XMMATRIX view = mSceneData.GetViewMatrix();
	XMMATRIX proj = mSceneData.GetProjMatrix();

	ObjectConstants obj;
	XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(world * view * proj));
	XMStoreFloat4x4(&obj.World, XMMatrixTranspose(world));
	XMMATRIX invWorld = XMMatrixInverse(nullptr, world);
	XMStoreFloat4x4(&obj.WorldInvTranspose, XMMatrixTranspose(invWorld));
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
			OutputDebugStringA("L'entité a déjà une texture associée.\n");
			return BuildMesh(vertices, indices, index);
		}

		std::ifstream testFile(filename);
		if (!testFile.good())
		{
			OutputDebugStringA("Le code C++ ne voit PAS le fichier à ce chemin !!!\n");
		}
		else
		{
			OutputDebugStringA("Le fichier est bien visible par le C++.\n");
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

	return *mGeoB;
}