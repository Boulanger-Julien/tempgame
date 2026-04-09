#include "pch.h"
static FLOAT2 MousePosSet;

char InputSystem::GetKeyDown()
{
	for (int i = 0; i < 256; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
		{
			return i;
		}
	}
}

bool InputSystem::isKeyDown()
{
	for (int i = 0; i < 256; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
		{
			return true;
		}
	}
	return false;
}

bool InputSystem::isKeyDown(char key)
{
	if (GetAsyncKeyState(key) & 0x8000)
	{
		return true;
	}
	return false;
}

bool InputSystem::isKeyUp(char key)
{
	static bool keysPressed[256] = { false };
	unsigned char k = (unsigned char)key;

	if (GetAsyncKeyState(key) & 0x8000)
	{
		keysPressed[k] = true;
		return false;
	}
	else if (keysPressed[k])
	{
		keysPressed[k] = false;
		return true;
	}
	return false;
}

float InputSystem::GetMouseX()
{
	POINT p;
	GetCursorPos(&p);
	return (float)p.x;
}

float InputSystem::GetMouseY()
{
	POINT p;
	GetCursorPos(&p);
	return (float)p.y;
}

FLOAT2 InputSystem::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	return FLOAT2((float)p.x, (float)p.y);
}

float InputSystem::GetMouseDeltaX()
{
	POINT p;
	GetCursorPos(&p);

	float deltaX = (float)p.x - MousePosSet.x;
	return (float)deltaX;
}

float InputSystem::GetMouseDeltaY()
{
	POINT p;
	GetCursorPos(&p);

	float deltaY = (float)p.y - MousePosSet.y;
	return (float)deltaY;
}

FLOAT2 InputSystem::GetMouseDelta()
{
	POINT p;
	GetCursorPos(&p);

	float deltaX = (float)p.x - MousePosSet.x;
	float deltaY = (float)p.y - MousePosSet.y;

	return FLOAT2(deltaX, deltaY);
}

void InputSystem::SetMousePos(float x, float y)
{
	SetCursorPos((int)x, (int)y);
	MousePosSet = FLOAT2(x, y);
}

void InputSystem::HideCursor(bool hide)
{
	if (hide)
	{
		while (ShowCursor(!hide) >= 0);
	}
	else
	{
		while (ShowCursor(hide) < 0);
	}
}