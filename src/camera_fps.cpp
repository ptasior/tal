#include "camera_fps.h"
#include "log.h"
#include "time.h"
#include "global.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void FpsCamera::init()
{
	Camera::init();

	// Initial position to show what is in 0,0,0
	mCameraPos = glm::vec3(0.0, 0.0, 1);
	// mCameraPos = glm::vec3(0.0, 1.0, 4);
	// mRotX = 4.6;
	// mRotY = -0.3;

	update();
}

bool FpsCamera::processEvents(const Uint8 *state)
{
	bool ret = false;
	const float speed = 0.2;

	if(state[SDL_SCANCODE_W])
	{
		mCameraPos.z += speed*sin(mRotX)*Global::get<Time>()->elapsed();
		mCameraPos.y += speed*sin(mRotY)*Global::get<Time>()->elapsed();
		mCameraPos.x += speed*cos(mRotX)*Global::get<Time>()->elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_S])
	{
		mCameraPos.z -= speed*sin(mRotX)*Global::get<Time>()->elapsed();
		mCameraPos.y -= speed*sin(mRotY)*Global::get<Time>()->elapsed();
		mCameraPos.x -= speed*cos(mRotX)*Global::get<Time>()->elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_A])
	{
		mCameraPos.z += speed*sin(mRotX-M_PI_2)*Global::get<Time>()->elapsed();
		mCameraPos.x += speed*cos(mRotX-M_PI_2)*Global::get<Time>()->elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_D])
	{
		mCameraPos.z += speed*sin(mRotX+M_PI_2)*Global::get<Time>()->elapsed();
		mCameraPos.x += speed*cos(mRotX+M_PI_2)*Global::get<Time>()->elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_Z])
	{
		mCameraPos.y += speed*Global::get<Time>()->elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_X])
	{
		mCameraPos.y -= speed*Global::get<Time>()->elapsed();
		ret = true;
	}

	if(state[SDL_SCANCODE_LCTRL])
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		if(mCaptureMouse)
		{
			mRotX += glm::radians(1.0*(x - mLastMX));
			mRotY += glm::radians(1.0*(mLastMY - y));

			mRotY = glm::clamp(mRotY, -M_PI_2+0.001, M_PI_2-0.001);
		}
		mLastMX = x;
		mLastMY = y;


		ret = true;
	}
	mCaptureMouse = state[SDL_SCANCODE_LCTRL];

	if(ret) update();
	return ret;
}

void FpsCamera::update()
{
	glm::vec3 front(cos(mRotX) * cos(mRotY),
					sin(mRotY),
					sin(mRotX) * cos(mRotY));

	mView = glm::lookAt(mCameraPos,
						mCameraPos + glm::normalize(front),
						glm::vec3(0.0, 1.0, 0.0));
}

