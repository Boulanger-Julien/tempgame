#include "pch.h"
#include "Window.h"
#include <random>

namespace {
	// Map l'ID de l'entité (ex: 5000) vers un Slot de descripteur (ex: 12)
	static std::unordered_map<int, int> sEntityToDescriptor;
	// Indique quel index de texture est utilisé pour un ID d'entité
	static std::unordered_map<int, int> sIndexUseTexture;
	// Pile des slots de descripteurs disponibles (1 à 1023)
	static std::vector<int> sFreeDescriptors;
	static bool sIsInitialized = false;
	constexpr int MaxDescriptors = 2048;
}

Window::Window(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

Window::~Window()
{
}
ID3D12Device* Window::GetDevice() const
{
	return md3dDevice.Get();
}
bool Window::Initialize(int winW, int winH)
{
	std::srand(static_cast<unsigned int>(time(0)));
	if (!D3DApp::Initialize(winW, winH))
		return false;

	if (!sIsInitialized) {
		// On remplit la pile à partir de 10 jusqu'à 1023
		// Les slots 0-9 sont ainsi protégés du recyclage automatique
		for (int i = MaxDescriptors - 1; i >= 10; i--) {
			sFreeDescriptors.push_back(i);
		}

		// On force manuellement le Splash (ID 0) sur le slot 0
		sEntityToDescriptor[0] = 0;

		sIsInitialized = true;
	}

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	mPipelineManager.Initialize(md3dDevice.Get(), mBackBufferFormat,
		mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);

	mDescriptorManager.Initialize(md3dDevice.Get(), MaxDescriptors);

	mBoxCount = 1;

	// Chargement du Splash Screen
	{
		// InitUI appellera BuildMesh, qui verra que l'ID 0 a déjà un slot (le 0)
		mSplashScreenMesh = std::make_unique<MeshGeometry>(InitUI(L"SplashScreen.dds", 0, 1920, 1080, XMFLOAT4(1, 1, 1, 1)));

		ExecuteInitCommands();
		DrawSplash();
		FlushCommandQueue();
		ResetCommandList();
	}

	return true;
}
void Window::RemoveEntityResources(int entityID) {
	// 1. Libérer le slot de descripteur
	auto itDesc = sEntityToDescriptor.find(entityID);
	if (itDesc != sEntityToDescriptor.end()) {
		sFreeDescriptors.push_back(itDesc->second); // Remet le slot dans la pile
		sEntityToDescriptor.erase(itDesc);
	}

	// 2. Libérer l'index de texture
	auto itTex = sIndexUseTexture.find(entityID);
	if (itTex != sIndexUseTexture.end()) {
		sIndexUseTexture.erase(itTex);
	}
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
	if (sEntityToDescriptor.find(renderIndex) == sEntityToDescriptor.end()) {
		return; // Évite le crash si l'entité n'est pas encore prête
	}

	int descriptorSlot = sEntityToDescriptor[renderIndex];
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

	obj.UseTexture = (sIndexUseTexture[renderIndex] > 0) ? sIndexUseTexture[renderIndex] : 0;

	// On met à jour le slot 'descriptorSlot' (0-1023) et non 'entityID' (5000+)
	mDescriptorManager.UpdateObjectConstants(descriptorSlot, obj);
}

void Window::UpdateUI(int renderIndex, XMMATRIX world)
{
	// SÉCURITÉ : On vérifie si l'ID existe dans notre système de slots
	if (sEntityToDescriptor.find(renderIndex) == sEntityToDescriptor.end()) {
		return;
	}

	int descriptorSlot = sEntityToDescriptor[renderIndex];
	ObjectConstants obj;

	float virtualWidth = 1920.0f;
	float virtualHeight = 1080.0f;

	XMMATRIX orthoProj = XMMatrixOrthographicOffCenterLH(0, virtualWidth, virtualHeight, 0, 0.0f, 1.0f);
	XMMATRIX vvp = world * orthoProj;

	XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(vvp));
	obj.UseLight = 0;

	// Utilisation sécurisée de l'index de texture
	obj.UseTexture = (sIndexUseTexture.count(renderIndex)) ? sIndexUseTexture[renderIndex] : 0;

	// CRITIQUE : Utiliser 'descriptorSlot' et non 'renderIndex'
	mDescriptorManager.UpdateObjectConstants(descriptorSlot, obj);
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

void Window::Draw(MeshGeometry& mGeo, int entityID) {
	if (mGeo.VertexBufferGPU == nullptr) return;
	if (sEntityToDescriptor.find(entityID) == sEntityToDescriptor.end()) return;
	mCommandList->SetPipelineState(mPipelineManager.GetPSO());

	int descriptorSlot = sEntityToDescriptor[entityID];
	int textureIdx = sIndexUseTexture[entityID];

	// Calcul de l'offset de texture dans le heap
	int textureDescriptorOffset = mDescriptorManager.GetMaxDescriptors() + textureIdx;

	mRenderContext.DrawMesh(
		mCommandList.Get(),
		md3dDevice.Get(),
		mGeo,
		descriptorSlot, // Utilise le slot 0-1023
		mDescriptorManager.GetMaxDescriptors(),
		textureDescriptorOffset,
		mDescriptorManager.GetDescriptorHeap()
	);
}
/**/
void Window::AddRenderItem(Mesh* mesh, /*Material* mat,*/ XMFLOAT4X4 world)
{
	//RenderItem* item = new RenderItem();
	//item->mesh = mesh;
	////item->material = mat;
	//item->world = world;

	//mRenderItems.push_back(item);
}
void Window::DrawRenderItems(RenderItem _renderItem)
{
	MeshGeometry mGeo = *_renderItem.mesh;
	int entityID = _renderItem.mEntityId;

	if (mGeo.VertexBufferGPU == nullptr) return;
	if (sEntityToDescriptor.find(entityID) == sEntityToDescriptor.end()) return;
	mCommandList->SetPipelineState(mPipelineManager.GetPSO());

	int descriptorSlot = sEntityToDescriptor[entityID];
	int textureIdx = sIndexUseTexture[entityID];

	// Calcul de l'offset de texture dans le heap
	int textureDescriptorOffset = mDescriptorManager.GetMaxDescriptors() + textureIdx;

	mRenderContext.DrawMesh(
		mCommandList.Get(),
		md3dDevice.Get(),
		mGeo,
		descriptorSlot, // Utilise le slot 0-1023
		mDescriptorManager.GetMaxDescriptors(),
		textureDescriptorOffset,
		mDescriptorManager.GetDescriptorHeap()
	);
}
//
void Window::DrawUI(MeshGeometry& mGeo, int entityID)
{
	if (mGeo.VertexBufferGPU == nullptr) return; // FIX

	// Pour l'UI, si c'est le splash (ID 0), on force le slot 0
	int descriptorSlot = (sEntityToDescriptor.count(entityID)) ? sEntityToDescriptor[entityID] : entityID;

	mCommandList->SetPipelineState(mPipelineManager.GetPSO_UI());
	int textureIdx = (sIndexUseTexture.count(entityID)) ? sIndexUseTexture[entityID] : 0;
	int textureDescriptorIndex = mDescriptorManager.GetMaxDescriptors() + textureIdx;

	mRenderContext.DrawMesh(
		mCommandList.Get(),
		md3dDevice.Get(),
		mGeo,
		descriptorSlot,
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

MeshGeometry Window::BuildMesh(std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, int entityID, const wchar_t* filename) {

	// --- 1. GESTION DU SLOT DE DESCRIPTEUR (RECYCLAGE) ---
	if (sEntityToDescriptor.find(entityID) == sEntityToDescriptor.end()) {
		if (!sFreeDescriptors.empty()) {
			int slot = sFreeDescriptors.back();
			sFreeDescriptors.pop_back();
			sEntityToDescriptor[entityID] = slot;
		}
		else {
			OutputDebugStringA("CRITICAL ERROR: No more descriptor slots available!\n");
			return MeshGeometry();
		}
	}
	int currentDescriptorSlot = sEntityToDescriptor[entityID];

	// --- 2. GESTION DE LA TEXTURE (SI EXISTANTE) ---
	static int sNextTextureIndex = 1;
	if (filename != nullptr) {
		// Code pour charger la texture (seulement si pas déjà fait pour cet ID)
		if (sIndexUseTexture.find(entityID) == sIndexUseTexture.end()) {
			auto texture = std::make_unique<Texture>();
			texture->Filename = filename;
			texture->Name = "Text" + std::to_string(entityID);

			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
				mCommandList.Get(), texture->Filename.c_str(),
				texture->Resource, texture->UploadHeap));

			if (texture->Resource != nullptr) {
				UINT mCbvSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mDescriptorManager.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

				// On place la texture APRES les 1024 slots de constantes
				hDescriptor.Offset(mDescriptorManager.GetMaxDescriptors() + sNextTextureIndex, mCbvSize);

				auto desc = texture->Resource->GetDesc();
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = desc.Format;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = desc.MipLevels;

				md3dDevice->CreateShaderResourceView(texture->Resource.Get(), &srvDesc, hDescriptor);
				mTextures[texture->Name] = std::move(texture);

				sIndexUseTexture[entityID] = sNextTextureIndex;
				sNextTextureIndex++;
			}
		}
	}
	else {
		// Pas de texture : on met l'index à 0 (ou un index par défaut)
		sIndexUseTexture[entityID] = 0;
	}

	// --- 3. CRÉATION DES BUFFERS GÉOMÉTRIQUES ---
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);
	if (vbByteSize == 0 || ibByteSize == 0) {
		OutputDebugStringA("ERREUR : Tentative de création d'un buffer de taille 0 !\n");
		return MeshGeometry();
	}
	MeshGeometry mGeoB;
	mGeoB.Name = (filename != nullptr) ? "TexturedGeo" : "UnTexturedGeo";

	mGeoB.VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), vertices.data(), vbByteSize, mGeoB.VertexBufferUploader);
	mGeoB.IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), indices.data(), ibByteSize, mGeoB.IndexBufferUploader);

	mGeoB.VertexByteStride = sizeof(Vertex);
	mGeoB.VertexBufferByteSize = vbByteSize;
	mGeoB.IndexFormat = DXGI_FORMAT_R32_UINT;
	mGeoB.IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	mGeoB.DrawArgs["box"] = submesh;

	return mGeoB;
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
		DrawUI(*mSplashScreenMesh, 0);
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
void Window::RegisterExistingMeshForEntity(int entityID) {
	if (sEntityToDescriptor.find(entityID) == sEntityToDescriptor.end()) {
		if (!sFreeDescriptors.empty()) {
			int slot = sFreeDescriptors.back();
			sFreeDescriptors.pop_back();
			sEntityToDescriptor[entityID] = slot;
			// On peut aussi lui assigner une texture par défaut (index 0)
			sIndexUseTexture[entityID] = 0;
		}
	}
}