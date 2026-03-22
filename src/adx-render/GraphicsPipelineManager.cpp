#include "pch.h"
#include "GraphicsPipelineManager.h"

/**/
void GraphicsPipelineManager::Init(ID3D12Device* _device)
{
	D3D12_ROOT_PARAMETER root_parameters[3] = {};

	// b0 - cbPerObject
	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	root_parameters[0].Descriptor.ShaderRegister = 0;
	root_parameters[0].Descriptor.RegisterSpace = 0;
	root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// b1 - cbLight
	root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	root_parameters[1].Descriptor.ShaderRegister = 1;
	root_parameters[1].Descriptor.RegisterSpace = 0;
	root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_DESCRIPTOR_RANGE srv_range = {};
	srv_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srv_range.NumDescriptors = 1;
	srv_range.BaseShaderRegister = 0;
	srv_range.RegisterSpace = 0;
	srv_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	root_parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	root_parameters[2].DescriptorTable.NumDescriptorRanges = 1;
	root_parameters[2].DescriptorTable.pDescriptorRanges = &srv_range;
	root_parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
	root_signature_desc.NumParameters = _countof(root_parameters);
	root_signature_desc.pParameters = root_parameters;
	root_signature_desc.NumStaticSamplers = 1;
	root_signature_desc.pStaticSamplers = &sampler;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_blob);
	if (FAILED(hr))
	{
		if (error_blob) OutputDebugStringA((char*)error_blob->GetBufferPointer());
		OutputDebugStringA("Root signature serialization FAILED\n");
		return;
	}

	hr = _device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create root signature\n");
		return;
	}

	if (signature_blob) signature_blob->Release();
	if (error_blob) error_blob->Release();

	ID3DBlob* vertex_shader = nullptr;
	ID3DBlob* pixel_shader = nullptr;

	ID3DBlob* vs_error = nullptr;
	ID3DBlob* ps_error = nullptr;

	hr = D3DCompileFromFile(L"..\\..\\res\\Shaders\\color.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0", 0, 0, &vertex_shader, &vs_error);
	if (FAILED(hr))
	{
		if (vs_error) OutputDebugStringA((char*)vs_error->GetBufferPointer());
		OutputDebugStringA("Vertex shader compilation FAILED\n");
		return;
	}

	hr = D3DCompileFromFile(L"..\\..\\res\\Shaders\\color.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0", 0, 0, &pixel_shader, &ps_error);
	if (FAILED(hr))
	{
		if (ps_error) OutputDebugStringA((char*)ps_error->GetBufferPointer());
		OutputDebugStringA("Pixel shader compilation FAILED\n");
		return;
	}

	if (vs_error) vs_error->Release();
	if (ps_error) ps_error->Release();

	pso_desc = {};
	pso_desc.pRootSignature = mRootSignature;
	pso_desc.VS.pShaderBytecode = vertex_shader->GetBufferPointer();
	pso_desc.VS.BytecodeLength = vertex_shader->GetBufferSize();
	pso_desc.PS.pShaderBytecode = pixel_shader->GetBufferPointer();
	pso_desc.PS.BytecodeLength = pixel_shader->GetBufferSize();

	set_blend_state(pso_desc.BlendState);
	pso_desc.SampleMask = UINT_MAX;
	set_rasterizer_state(pso_desc.RasterizerState);
	SetPsoDepthStencilState();

	// Input layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,28,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};
	pso_desc.InputLayout.pInputElementDescs = inputLayout;
	pso_desc.InputLayout.NumElements = _countof(inputLayout);

	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;

	hr = _device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&mPipelineState));
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create PSO\n");
		return;
	}

	// Release shaders (PSO has a copy)
	if (vertex_shader) vertex_shader->Release();
	if (pixel_shader) pixel_shader->Release();
}

void GraphicsPipelineManager::set_blend_state(D3D12_BLEND_DESC& blend_desc) {
	blend_desc = {};

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC default_render_target_blend_desc = {};
	default_render_target_blend_desc.BlendEnable = FALSE;
	default_render_target_blend_desc.LogicOpEnable = FALSE;
	default_render_target_blend_desc.SrcBlend = D3D12_BLEND_ONE;
	default_render_target_blend_desc.DestBlend = D3D12_BLEND_ZERO;
	default_render_target_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
	default_render_target_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
	default_render_target_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
	default_render_target_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	default_render_target_blend_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
	default_render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
		blend_desc.RenderTarget[i] = default_render_target_blend_desc;
	}
}

void GraphicsPipelineManager::set_rasterizer_state(D3D12_RASTERIZER_DESC& rasterizer_desc) {
	rasterizer_desc = {};

	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void GraphicsPipelineManager::SetPsoDepthStencilState() {
	D3D12_DEPTH_STENCIL_DESC _depthStencilState = {};
	_depthStencilState.DepthEnable = TRUE;
	_depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	_depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	_depthStencilState.StencilEnable = FALSE;
	_depthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	_depthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	pso_desc.DepthStencilState = _depthStencilState;
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
}

void GraphicsPipelineManager::Bind(ID3D12GraphicsCommandList* cmd)
{
	cmd->SetGraphicsRootSignature(mRootSignature);
	cmd->SetPipelineState(mPipelineState);
}
//
void GraphicsPipelineManager::Initialize(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality)
{
	BuildRootSignature(device);
	BuildShadersAndInputLayout();
	BuildPSO(device, backBufferFormat, depthStencilFormat, msaaState, msaaQuality);
}
void GraphicsPipelineManager::BuildRootSignature(ID3D12Device* device)
{
	CD3DX12_DESCRIPTOR_RANGE texRange;
	texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvRangeObj;
	cbvRangeObj.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvRangeLight;
	cbvRangeLight.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvRangeObj);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvRangeLight);
	slotRootParameter[2].InitAsDescriptorTable(1, &texRange);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		3, slotRootParameter,
		1, &linearWrap,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void GraphicsPipelineManager::BuildShadersAndInputLayout()
{
	mvsByteCode = d3dUtil::CompileShader(L"..\\..\\res\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"..\\..\\res\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void GraphicsPipelineManager::BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, UINT msaaQuality)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = backBufferFormat;
	psoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = msaaState ? (msaaQuality - 1) : 0;
	psoDesc.DSVFormat = depthStencilFormat;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC uiPsoDesc = psoDesc; 

	uiPsoDesc.DepthStencilState.DepthEnable = FALSE;
	uiPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = uiPsoDesc.BlendState.RenderTarget[0];
	blendDesc.BlendEnable = TRUE;
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	ThrowIfFailed(device->CreateGraphicsPipelineState(&uiPsoDesc, IID_PPV_ARGS(&mPSO_UI)));
}
