#pragma once 
#include "framework.h"
#include "../adx-engine/ECS.h"
#include "../adx-engine/transform.h"
#include "../adx-core/d3dApp.h"
#include "Window.h"

class UIRenderer
{
public :
	UIRenderer(Window& win, int index, int width, int height, XMFLOAT4 color = XMFLOAT4(Colors::White), const wchar_t* filename = nullptr) {
		UIQuad = win.InitUI(filename, index, width, height, color);
	};
	MeshGeometry UIQuad;
private: 
};