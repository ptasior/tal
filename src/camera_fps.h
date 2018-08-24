#pragma once
#include "camera.h"

class FpsCamera : public Camera
{
public:
	void init();
	bool processEvents(const Uint8 *state);

private:
	void update();

	bool mCaptureMouse = false;
	double mRotX;
	double mRotY;
	int mLastMX;
	int mLastMY;
};

 
