#include "StdAfx.h"
#include "FPS.h"



void FpsData::Reset()
{
	startTime = timeGetTime();
	frameCount = 0;
	lastReading = 0.0;
	valid = false;
}

void FpsData::NewFrame(DWORD timeNow)
{
	frameCount++;

	if (timeNow - startTime > 1000)
	{
		double seconds = (float)(timeNow - startTime) / 1000.0;
		lastReading = (float)frameCount / seconds;
		valid = true;
		startTime = timeNow;
		frameCount = 0;
	}
}