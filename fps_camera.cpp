#include "fps_camera.h"
#include "log.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void FpsCamera::init()
{
	Log() << "FpsCamera init";

	// Initial position to show what is in 0,0,0
	cameraPos = glm::vec3(0.0, 0.0, 1);
	mRotX = 4.6;

	update();
}

bool FpsCamera::event(SDL_Event &event)
{
	if(event.type == SDL_KEYDOWN)
		switch (event.key.keysym.sym)
		{
			case SDLK_w:
				cameraPos.z += 0.05*sin(mRotX);
				cameraPos.y += 0.05*sin(mRotY);
				cameraPos.x += 0.05*cos(mRotX);
				update();
				return true;
			case SDLK_s:
				cameraPos.z -= 0.05*sin(mRotX);
				cameraPos.y -= 0.05*sin(mRotY);
				cameraPos.x -= 0.05*cos(mRotX);
				update();
				return true;
			case SDLK_a:
				cameraPos.z += 0.05*sin(mRotX-M_PI_2);
				cameraPos.x += 0.05*cos(mRotX-M_PI_2);
				update();
				return true;
			case SDLK_d:
				cameraPos.z += 0.05*sin(mRotX+M_PI_2);
				cameraPos.x += 0.05*cos(mRotX+M_PI_2);
				update();
				return true;
			case SDLK_LCTRL:
				mCaptureMouse = true;
				SDL_GetMouseState(&mLastMX, &mLastMY);
				return true;
		}
	if(event.type == SDL_KEYUP)
		switch (event.key.keysym.sym)
		{
			case SDLK_LCTRL:
				mCaptureMouse = false;
				return true;
		}

	if(mCaptureMouse && event.type == SDL_MOUSEMOTION)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		mRotX += 0.01*(x - mLastMX);
		mRotY += 0.01*(mLastMY - y);
		mLastMX = x;
		mLastMY = y;

		update();
		return true;
	}

	return false;
}

void FpsCamera::update()
{
    glm::vec3 front(cos(mRotX) * cos(mRotY),
					sin(mRotY),
					sin(mRotX) * cos(mRotY));

	mView = glm::lookAt(cameraPos,
						cameraPos + glm::normalize(front),
						glm::vec3(0.0, 1.0, 0.0));
}

