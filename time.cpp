#include "time.h"
#include "gl_header.h" // For STL

float Time::mCurr;
float Time::mPrev;
float Time::mFps;
std::mutex Time::mMutex;

void Time::registerNextFrame()
{
	std::lock_guard<std::mutex> lock(mMutex);
	mPrev = mCurr;
	mCurr = SDL_GetTicks();
	mFps = 0.9*mFps + 100.0/(mCurr-mPrev);
}

float Time::elapsed()
{
	std::lock_guard<std::mutex> lock(mMutex);
	return (mCurr - mPrev)/100.0;
}

float Time::fps()
{
	return mFps;
}

