#pragma once 
#include "d3dx12.h"
#include "d3d12.h"
#include "DDSTextureLoader.h"
#include "UploadBuffer.h"
#include "Vertex.h"
#include "Objects.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "GraphicsPipelineManager.h"
#include "DescriptorManager.h"
#include "SceneData.h"
#include "RenderContext.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "windowscodecs.lib") // Tr�s important pour le chargement d'images