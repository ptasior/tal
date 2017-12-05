#pragma once
#include "camera.h"

class RotatingCamera : public Camera
{
public:
	void init();
	void apply(GLuint uniform_mvp);
};

 
