#pragma once
#include "camera.h"

class RotatingCamera : public Camera
{
public:
	void init();
	void update();

	void setCenter(const glm::vec3 &c);
	glm::vec3 getCenter();

	bool processEvents(const Uint8 *state);
	bool onEvent(SDL_Event &event);

private:
	glm::vec3 mCenter;
	glm::mat4 mSPrerot;

	bool mCaptureMouse = false;
	double mRotX;
	double mRotY;
	double mDistance;
	int mLastMX;
	int mLastMY;
};

 
