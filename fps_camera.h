#pragma once
#include "camera.h"

class FpsCamera : public Camera
{
public:
	void init();
	bool event(SDL_Event &event);
private:
	void update();

	bool mCaptureMouse = false;
	float mRotX;
	float mRotY;
	glm::vec3 cameraPos;
	int mLastMX;
	int mLastMY;
};

 
