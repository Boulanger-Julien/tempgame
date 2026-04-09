#include "pch.h"

Timer* Timer::instance = nullptr;

void Timer::Reset()
{
	if (instance == nullptr)
	{
		instance = new Timer();
	}
	instance->m_startTime = std::chrono::high_resolution_clock::now();
	instance->m_deltaTime = 0.0f;
	instance->m_totalTime = 0.0f;
	instance->m_paused = false;
}

float Timer::GetDeltatime()
{
	if (instance->m_paused)
	{
		return 0.0f;
	}
	else
	{
		return instance->m_deltaTime;
	}
}

float Timer::GetTotalTime()
{
	if (instance->m_paused)
	{
		return 0.0f;
	}
	else
	{
		return instance->m_totalTime;
	}
}

int Timer::GetFramerate()
{
	return 1.0f / instance->m_deltaTime;
}

void Timer::Pause(bool pause)
{
	if (instance == nullptr)
	{
		instance = new Timer();
	}
	instance->m_paused = pause;
}

void Timer::Update()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	instance->m_deltaTime = std::chrono::duration<float>(currentTime - instance->m_startTime).count();
	instance->m_startTime = currentTime;
	instance->m_totalTime += instance->m_deltaTime;
}

Timer* Timer::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new Timer();
	}
	return instance;
}
