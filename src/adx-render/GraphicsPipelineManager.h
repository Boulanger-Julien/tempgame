#pragma once
#include "framework.h"

class GraphicsPipelineManager
{
public:
	GraphicsPipelineManager() = default;
	~GraphicsPipelineManager() = default;

	void Initialize(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);
	
	ID3D12RootSignature* GetRootSignature() const { return mRootSignature.Get(); }
	ID3D12PipelineState* GetPSO() const { return mPSO.Get(); }
	ID3D12PipelineState* GetPSO_UI() const { return mPSO_UI.Get(); }

private:
	void BuildRootSignature(ID3D12Device* device);
	void BuildShadersAndInputLayout();
	void BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	Microsoft::WRL::ComPtr< ID3D12PipelineState> mPSO_UI = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
};

