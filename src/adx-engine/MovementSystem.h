#pragma once
#include "ECS.h"
class MovementSystem
{
private:
public:
	MovementSystem() {}
	~MovementSystem() {}
	void Update(float deltaTime, ECS ecs);
	void Move(float deltaTime, ECS ecs);
};
