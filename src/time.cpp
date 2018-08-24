#include "time.h"
#include "gl_header.h" // For STL

void Time::registerNextFrame()
{
	mPrev = mCurr;
	mCurr = SDL_GetTicks();
	float denom = (mCurr-mPrev);
	if(denom <= 0) denom = 1;

	mFps = 0.9*mFps + 100.0/denom;
}

float Time::elapsed()
{
	return (mCurr - mPrev)/100.0;
}

float Time::fps()
{
	return mFps;
}

float Time::current()
{
	return mCurr;
}

