#pragma once 

#include "transform.h"
#include "ECS.h"

class InputSystem
{
public:
	InputSystem() {}
	~InputSystem() {}
	void Update();
	static char GetKeyDown();
	static bool isKeyDown(char key);
	static bool isKeyUp(char key);
	static float GetMouseX();
	static float GetMouseY();
	static FLOAT2 GetMousePos();
	static float GetMouseDeltaX();
	static float GetMouseDeltaY();
	static FLOAT2 GetMouseDelta();
	static void SetMousePos(float x, float y);
	static void HideCursor(bool hide);
};