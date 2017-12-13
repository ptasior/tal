#pragma once
#include <mutex>

class Time
{
public:
	static void registerNextFrame();
	static float elapsed();
	static float fps();

private:
	static float mCurr;
	static float mPrev;
	static float mFps;
	static std::mutex mMutex;
};

