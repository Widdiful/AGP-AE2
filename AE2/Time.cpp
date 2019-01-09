#include "Time.h"
#include <windows.h>



Time::~Time()
{
}

void Time::Tick()
{
	frameCount++;
	timeNow = double(timeGetTime());
	deltaTime = timeNow - timePrevious;
	timePrevious = timeNow;

	float deltaTimeSeconds = deltaTime / 1000.0f;

	if (frameCount % 10 == 0) {
		fps = 1.0f / deltaTimeSeconds;
		fpsInt = fps;
	}

	if (deltaTime > 1000) deltaTime = 0;
}
