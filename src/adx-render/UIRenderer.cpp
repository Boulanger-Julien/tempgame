#include "pch.h"
#include "UIRenderer.h"
#include "adx-shooter/GameManager.h"

UIRenderer::UIRenderer(int index, int width, int height, XMFLOAT4 color, const wchar_t* filename)
{
	Window* w = GameManager::GetInstance().GetWindow();
	UIQuad = w->InitUI(filename, index, width, height, color);
}