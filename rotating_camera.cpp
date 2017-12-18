#include "rotating_camera.h"
#include "log.h"
#include "time.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void RotatingCamera::init()
{
	Camera::init();
	mCameraPos = glm::vec3(1.0, 0.0, 1.0);
	mCenter = glm::vec3(1.0, -0.5, 1.0);
	mDistance = 2.0;
	update();
}

void RotatingCamera::update()
{
	glm::vec3 front(cos(mRotX) * cos(mRotY),
					sin(mRotY),
					sin(mRotX) * cos(mRotY));

	front = glm::normalize(front) * (float)mDistance;

	mCameraPos = mCenter-front;

	setView(glm::lookAt(mCameraPos,
				mCenter,
				glm::vec3(0.0, 1.0, 0.0))
			);
}

void RotatingCamera::setCenter(const glm::vec3 &c)
{
	mCenter = c;
}

glm::vec3 RotatingCamera::getCenter()
{
	return mCenter;
}

bool RotatingCamera::onEvent(SDL_Event &event)
{
	const float speed = 1;
	switch (event.type)
	{
		case SDL_MOUSEWHEEL:
			if(event.wheel.y == 1) // scroll up
				mDistance -= speed*Time::elapsed();
			else if(event.wheel.y == -1) // scroll down
				mDistance += speed*Time::elapsed();
			update();
			return true;
	}
	return false;
}

bool RotatingCamera::processEvents(const Uint8 *state)
{
	bool ret = false;
	const float speed = 0.2;

	if(state[SDL_SCANCODE_W])
	{
		// mCenter.x += speed*Time::elapsed();
		mCenter.z += speed*sin(mRotX)*Time::elapsed();
		mCenter.y += speed*sin(mRotY)*Time::elapsed();
		mCenter.x += speed*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_S])
	{
		mCenter.z -= speed*sin(mRotX)*Time::elapsed();
		mCenter.y -= speed*sin(mRotY)*Time::elapsed();
		mCenter.x -= speed*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_A])
	{
		mCenter.z += speed*sin(mRotX-M_PI_2)*Time::elapsed();
		mCenter.x += speed*cos(mRotX-M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_D])
	{
		mCenter.z += speed*sin(mRotX+M_PI_2)*Time::elapsed();
		mCenter.x += speed*cos(mRotX+M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_Z])
	{
		mCenter.y += speed*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_X])
	{
		mCenter.y -= speed*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_E])
	{
		ret = true;
	}
	if(state[SDL_SCANCODE_R])
	{
		mDistance -= 0.1;
		ret = true;
	}

	int x, y;
	int mstate = SDL_GetMouseState(&x, &y);

	if(mstate & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		mRotX += glm::radians(1.0*(x - mLastMX));
		mRotY += glm::radians(1.0*(mLastMY - y));

		mRotY = glm::clamp(mRotY, -M_PI_2+0.01, -0.1);
		ret = true;
	}
	mLastMX = x;
	mLastMY = y;

	

	if(ret) update();
	return ret;
}

