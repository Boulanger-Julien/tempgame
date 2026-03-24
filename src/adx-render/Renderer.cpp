#include "pch.h"
#include "Renderer.h"

void Renderer::Run(HWND& _hwnd, int _width, int _height) {
	mWindowWidth = _width;
	mWindowHeight = _height;

	Init(_hwnd);
	Loop();
}
void Renderer::Init(HWND& _hwnd) {

	mDeviceManager.Init();
	mCommandManager.Init(mDeviceManager.GetDevice());
	mGraphicPipelineManager.Init(mDeviceManager.GetDevice());
	mSwapChainManager.Init(_hwnd, mWindowWidth, mWindowHeight, mDeviceManager.GetDevice(), mCommandManager.GetCommandQueue());
	mDepthBufferManager.Init(mDeviceManager.GetDevice(), mWindowWidth, mWindowHeight);
	mFenceManager.Init(mDeviceManager.GetDevice());

	mCamera.SetLens(0.25f * 3.14, (mWindowWidth / mWindowHeight), 0.1f, 100.0f);
	mCamera.SetPosition(0.0f, 3.0f, -15.0f);

	//Light
		//mLight = Light(XMFLOAT3(1.0f, -1.0f, 0.0f), 1, XMFLOAT4(1.0f, 1.f, 1.0f, 1.0f));
		//mSceneData.SetLight(mLight);
		//LightConstants lc = mSceneData.GetLightConstants();
		//mDescriptorManager.UpdateLightConstants(lc);
	//
	mRessourceManager.Init(mDeviceManager.GetDevice());

	//mRessourceManager.LoadTexture(mDeviceManager.GetDevice(), "Cube", L"Diamond2.dds");
	//Texture* cubeText = mRessourceManager.GetTexture("Cube"); // récupère la même instance

	RenderItem cubeItem;
	cubeItem.CreateItem("Cube", mRessourceManager.GetCubeMesh());
	CreateConstantBuffer(cubeItem);

	mRenderItems.push_back(cubeItem);
}
void Renderer::Loop() {
	bool running = true;
	while (running) {

		running = ProcessMessages();

		Update(GetDeltaTime());

		FRender();

		ID3D12CommandList* command_lists[] = { mCommandManager.GetCommandList() };
		mCommandManager.GetCommandQueue()->ExecuteCommandLists(1, command_lists);

		mSwapChainManager.GetSwapChain()->Present(1, 0);

		const UINT64 current_fence_value = mFenceManager.IncrementFenceValue();
		mCommandManager.GetCommandQueue()->Signal(mFenceManager.GetFence(), current_fence_value);

		if (mFenceManager.GetFence()->GetCompletedValue() < current_fence_value) {
			mFenceManager.GetFence()->SetEventOnCompletion(current_fence_value, mFenceManager.GetFenceEvent());
			WaitForSingleObject(mFenceManager.GetFenceEvent(), INFINITE);
		}
	}
}
bool Renderer::ProcessMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}
float Renderer::GetDeltaTime()
{
	auto now = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(now - m_previousTime).count();
	m_previousTime = now;
	elapsedTime += deltaTime;

	return deltaTime;
}
void Renderer::Update(float _deltaTime) {
	//scale
	float sx = 0.5;
	float sy = 0.5;
	float sz = 0.5;
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(sx, sy, sz);

	//cube
	DirectX::XMMATRIX cubePosition = DirectX::XMMatrixTranslation(0,2,3);
	DirectX::XMMATRIX cubeWorld = cubePosition;

	//view//
	DirectX::XMMATRIX view = mCamera.GetViewMatrix();
	//proj
	DirectX::XMMATRIX projection = mCamera.GetProjMatrix();

	//UpdateCamera
	mCamera.Update(_deltaTime);

	//UpdateViewMatrix
	mCamera.UpdateViewMatrix();

	for (auto item : mRenderItems)
	{
		DirectX::XMMATRIX world;

		if (item.name == "Cube")
			world = cubeWorld;

		DirectX::XMMATRIX wvp = world * view * projection;

		ObjectConstants cb;
		cb.UseLight = false;
		DirectX::XMStoreFloat4x4(&cb.WorldViewProj,
			DirectX::XMMatrixTranspose(wvp));

		void* data;
		D3D12_RANGE readRange = {};
		item.constantBuffer->Map(0, &readRange, &data);
		memcpy(data, &cb, sizeof(cb));
		item.constantBuffer->Unmap(0, nullptr);
	}
}

void Renderer::FRender() {
	mCommandManager.BeginFrame();
	mSwapChainManager.BeginFrame(mCommandManager.GetCommandList());

	Clear();
	SetViewport();
	DrawScene();

	mSwapChainManager.EndFrame(mCommandManager.GetCommandList());
	mCommandManager.EndFrame();
}

void Renderer::Clear()
{
	mRtvHandle = mSwapChainManager.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
	mRtvHandle.ptr += mSwapChainManager.GetBackBufferIndex() * mSwapChainManager.GetRtvIncrementSize();

	float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mCommandManager.GetCommandList()->ClearRenderTargetView(mRtvHandle, clearColor, 0, nullptr);
	mCommandManager.GetCommandList()->ClearDepthStencilView(
		mDepthBufferManager.GetDsvHeap()->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr
	);
}
void Renderer::SetViewport()
{
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight), 0.0f, 1.0f };
	D3D12_RECT scissor_rect = { 0, 0, mWindowWidth, mWindowHeight };
	mCommandManager.GetCommandList()->RSSetViewports(1, &viewport);
	mCommandManager.GetCommandList()->RSSetScissorRects(1, &scissor_rect);
}
void Renderer::DrawScene()
{
	auto dsvHandle = mDepthBufferManager.GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	mCommandManager.GetCommandList()->OMSetRenderTargets(1, &mRtvHandle, FALSE, &dsvHandle);

	mCommandManager.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mGraphicPipelineManager.Bind(mCommandManager.GetCommandList());

	auto commandList = mCommandManager.GetCommandList();

	for (auto& item : mRenderItems)
	{
		item.mesh->Bind(commandList);

		// Juste passer le constant buffer de l'objet (cube)
		commandList->SetGraphicsRootConstantBufferView(
			0,
			item.constantBuffer->GetGPUVirtualAddress()
		);

		// Ne pas mettre de lumière ni de texture
		// commandList->SetGraphicsRootConstantBufferView(1, mLightConstantBuffer->GetGPUVirtualAddress());
		// commandList->SetGraphicsRootDescriptorTable(2, mDiffuseMapGpuHandle);

		commandList->DrawIndexedInstanced(
			item.mesh->GetIndexCount(),
			1,  // instance count
			0,  // start index
			0,  // base vertex
			0   // start instance
		);
	}
}
void Renderer::CreateConstantBuffer(RenderItem& item)
{
	UINT alignedSize = (sizeof(ObjectConstants) + 255) & ~255;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = alignedSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.SampleDesc.Count = 1;

	mDeviceManager.GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&item.constantBuffer)
	);
}
