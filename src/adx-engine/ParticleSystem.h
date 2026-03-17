#pragma once
#include "ECS.h"
class IRenderer;

class ParticleSystem
{
public:
	ParticleSystem() {}
	~ParticleSystem() {}
	void Update(float deltaTime, ECS ecs);
private:
	IRenderer* m_renderer;

};