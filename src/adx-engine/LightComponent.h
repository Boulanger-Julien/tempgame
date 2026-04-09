#pragma once 
#include "transform.h"

struct LightComponent
{
	FLOAT3 color; // Initially White
	float intensity; // Initially 0.5f, so half-bright | Goes from 0 to 1

	bool lightSwitch = false; // False - light off | True - light on

	// Creates the Light
	LightComponent(FLOAT3 _color = FLOAT3(255.f, 255.f, 255.f), float _intensity = 0.5f) { color = _color; intensity = _intensity; }
};