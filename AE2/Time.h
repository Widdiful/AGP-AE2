#pragma once
class Time
{
private:
	double timeNow;
	double timePrevious;

public:
	Time();
	~Time();
	double timeSinceStart;
	double deltaTime;
	float fps;
	int fpsInt;
	void Tick();
};

