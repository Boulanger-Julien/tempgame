#pragma once
#include <chrono>
	
class Timer
{
public:
	Timer(){}

	static void Reset();

	static float GetDeltatime();
	static float GetTotalTime();
	static int GetFramerate();
	static void Pause(bool pause);
	static void Update();
	static Timer* GetInstance();


private:
	static Timer* instance;
	std::chrono::high_resolution_clock::time_point m_startTime;
	float m_deltaTime = 0.0f;
	float m_totalTime = 0.0f;
	bool m_paused = false;
};