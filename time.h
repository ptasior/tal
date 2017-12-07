#pragma once
#include <mutex>

class Time
{
public:
	static void registerNextFrame();
	static float elapsed();

private:
	static float mCurr;
	static float mPrev;
	static std::mutex mMutex;
};

