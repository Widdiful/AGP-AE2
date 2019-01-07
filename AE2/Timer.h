#pragma once
class Timer
{
private:
	double timeNow;
	double timePrevious;

public:
	Timer();
	~Timer();
	double timeSinceStart;
	double deltaTime;
	void Tick();
};

