#pragma once
#include <windows.h>

class Time
{
private:
	Time() {
		timeNow = double(timeGetTime());
		timeSinceStart = 0;
		deltaTime = 0;
		fps = 0;
		timePrevious = timeNow;
	}
	double timeNow;
	double timePrevious;
	int frameCount;

public:
	static Time& getInstance() {
		static Time instance;
		return instance;
	}
	~Time();
	double timeSinceStart;
	double deltaTime;
	float fps;
	int fpsInt;
	void Tick();
};

