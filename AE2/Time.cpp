#include "Time.h"
#include <windows.h>



Time::~Time()
{
}

void Time::Tick()
{
	timeNow = double(timeGetTime());
	deltaTime = timeNow - timePrevious;
	timePrevious = timeNow;

	float deltaTimeSeconds = deltaTime / 1000.0f;
	fps = (1.0f / deltaTimeSeconds) * 100.0f;
	fpsInt = fps;

	if (deltaTime > 1000) deltaTime = 0;
}
