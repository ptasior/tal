#pragma once
#include "camera.h"

class FpsCamera : public Camera
{
public:
	void init();
	bool processEvents(const Uint8 *state);
	void applySprite(GLuint uniform_mvp);

private:
	void update();

	bool mCaptureMouse = false;
	double mRotX;
	double mRotY;
	glm::vec3 cameraPos;
	int mLastMX;
	int mLastMY;
};

 
