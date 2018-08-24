#pragma once
#include <mutex>

class Time
{
public:
	// Call at begginig of rendering a new frame
	static void registerNextFrame();

	// Time elapsed since last frame
	static float elapsed();

	// Smmothened frames per sec
	static float fps();

	// Returns time of current frame
	static float current();

	// Returns SDL_GetTicks()
	static float realTime();

private:
	static float mCurr;
	static float mPrev;
	static float mFps;
	static std::mutex mMutex;
};

