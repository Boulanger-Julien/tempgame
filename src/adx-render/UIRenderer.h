#pragma once 
#include "framework.h"
#include "../adx-engine/ECS.h"
#include "../adx-engine/transform.h"
#include "../adx-core/d3dApp.h"
#include "Window.h"

class UIRenderer
{
public :
	UIRenderer(XMFLOAT4 color, const wchar_t* filename = nullptr);
	UIRenderer() = default;
	MeshGeometry UIQuad;
	void AddIndex(int index);
	void PushIndex();
private: 
	std::vector<int> mIndexToPush;
	std::wstring mFilename;
};