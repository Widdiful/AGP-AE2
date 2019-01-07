#include "Timer.h"
#include <windows.h>



Timer::Timer()
{
	timeNow = double(timeGetTime());
	timeSinceStart = 0;
	deltaTime = 0;
	timePrevious = timeNow;
}


Timer::~Timer()
{
}

void Timer::Tick()
{
	timeNow = double(timeGetTime());
	deltaTime = timeNow - timePrevious;
	timePrevious = timeNow;

	if (deltaTime > 1000) deltaTime = 0;
}
