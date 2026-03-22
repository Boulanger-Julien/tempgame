#pragma once
#include "framework.h"

class GraphicsPipelineManager
{
public:
	GraphicsPipelineManager() = default;
	~GraphicsPipelineManager() = default;

	void Initialize(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);

	void Init(ID3D12Device* device);

	ID3D12RootSignature* GetRootSignature() const { return mRootSignature; }
	ID3D12PipelineState* GetPSO() const { return mPSO; }
	ID3D12PipelineState* GetPSO_UI() const { return mPSO_UI.Get(); }

	/**/
	void Bind(ID3D12GraphicsCommandList* cmd);
	//

private:
	void BuildRootSignature(ID3D12Device* device);
	void BuildShadersAndInputLayout();
	void BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);


	ID3D12RootSignature* mRootSignature = nullptr;
	ID3D12PipelineState* mPSO = nullptr;
	Microsoft::WRL::ComPtr< ID3D12PipelineState> mPSO_UI = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ID3D12CommandQueue* mCommandQueue = nullptr;
	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;
};

