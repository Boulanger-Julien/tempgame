#include "pch.h"
#include "main.h"
#include "GameManager.h"
#include "Renderer.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	float width = 1080;
	float height = 920;
	std::wstring mMainWndCaption = L"Adx Engine";

	// Register a simple window class
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"DirectX12Triangle";
	RegisterClass(&wc);

	// Create a window
	HWND hwnd = CreateWindow(L"DirectX12Triangle", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, cmdShow);

	Renderer app;
	app.Run(hwnd, width, height);

	//try
	//{
	//	GameManager App(hInstance,1080,720);
	//	if (!App.Initialize())
	//		return 0;

	//	return App.Run();
	//}
	//catch (DxException& e)
	//{
	//	MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
	//	return 0;
	//}
}