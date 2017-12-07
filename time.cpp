#include "time.h"
#include "gl_header.h" // For STL

float Time::mCurr;
float Time::mPrev;
std::mutex Time::mMutex;

void Time::registerNextFrame()
{
	std::lock_guard<std::mutex> lock(mMutex);
	mPrev = mCurr;
	mCurr = SDL_GetTicks();
}

float Time::elapsed()
{
	std::lock_guard<std::mutex> lock(mMutex);
	return (mCurr - mPrev)/100.0;
}

