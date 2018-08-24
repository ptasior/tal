#pragma once

class Time
{
public:
	// Call at begginig of rendering a new frame
	void registerNextFrame();

	// Time elapsed since last frame
	float elapsed();

	// Smmothened frames per sec
	float fps();

	// Returns time of current frame
	float current();

	// Returns SDL_GetTicks()
	float realTime();

private:
	float mCurr;
	float mPrev;
	float mFps;
};

