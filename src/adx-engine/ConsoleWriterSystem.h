#pragma once
#include "ECS.h"
#include <string>
#include <iostream>

class ConsoleWriterSystem
{
private:
	float m_time = 0.0f;
public:
	ConsoleWriterSystem() {}
	~ConsoleWriterSystem() {}
	template<typename ...Args>
	void Update(ECS ecs, float dt, Args ...l);
};

template<typename... Args>
void ConsoleWriterSystem::Update(ECS ecs, float dt, Args... l)
{
	if (sizeof...(l) == 0)
	{
		return;
	}
	if (m_time >= 1.0f)
	{
		m_time = 0.0f;
		m_time += dt;
		(std::cout << ... << l) << std::endl;
	}
	else
	{
		m_time += dt;
		return;
	}
}

