#pragma once
#include "framework.h"

class GraphicsPipelineManager
{
public:
	GraphicsPipelineManager() = default;
	~GraphicsPipelineManager() = default;

	void Initialize(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);


	ID3D12RootSignature* GetRootSignature() const { return mRootSignature; }
	ID3D12PipelineState* GetPSO() const { return mPipelineState; }
	ID3D12PipelineState* GetPSO_UI() const { return mPSO_UI.Get(); }

	/**/
	void Init(ID3D12Device* _device);
	void set_blend_state(D3D12_BLEND_DESC& blend_desc);
	void set_rasterizer_state(D3D12_RASTERIZER_DESC& rasterizer_desc);
	void SetPsoDepthStencilState();
	void Bind(ID3D12GraphicsCommandList* cmd);
	//

private:
	void BuildRootSignature(ID3D12Device* device);
	void BuildShadersAndInputLayout();
	void BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);

	/**/
	ID3D12RootSignature* mRootSignature = nullptr;
	ID3D12PipelineState* mPipelineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
	//
	Microsoft::WRL::ComPtr< ID3D12PipelineState> mPSO_UI = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ID3D12CommandQueue* mCommandQueue = nullptr;
	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;
};

