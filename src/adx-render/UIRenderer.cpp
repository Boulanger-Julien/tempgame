#include "pch.h"
#include "UIRenderer.h"
#include "adx-shooter/GameManager.h"

UIRenderer::UIRenderer(XMFLOAT4 color, const wchar_t* filename)
{
	Window* w = GameManager::GetInstance().GetWindow();
	if (filename != nullptr)
	{
		mFilename = L"..\\..\\res\\Textures\\";
		mFilename += filename;
	}
	else 
		{
	}
	if (filename != nullptr)
	{
		UIQuad = w->InitUI(mFilename.c_str(), color);
	}
	else
		{
		UIQuad = w->InitUI(nullptr, color);
	}
}

void UIRenderer::AddIndex(int index)
{
	mIndexToPush.push_back(index);
}

void UIRenderer::PushIndex()
{
	for (int index : mIndexToPush)
	{
		if (!mFilename.empty())
		{
			GameManager::GetInstance().GetWindow()->SetTextureToIndex({ index }, mFilename.c_str());
		}
		else
		{
			GameManager::GetInstance().GetWindow()->SetTextureToIndex({ index }, nullptr);
		}
	}
	mIndexToPush.clear();
}