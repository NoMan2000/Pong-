

class FpsData
{
public:
	void Reset();
	void NewFrame(DWORD timeNow = timeGetTime());
	double LastReading() { return lastReading; }
	bool Valid()         { return valid; }
private:
	DWORD startTime;
	DWORD frameCount;
	double lastReading;
	bool valid;
};