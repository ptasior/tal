#include "fps_camera.h"
#include "log.h"
#include "time.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void FpsCamera::init()
{
	Log() << "FpsCamera: init";

	// Initial position to show what is in 0,0,0
	// cameraPos = glm::vec3(0.0, 0.0, 1);
	cameraPos = glm::vec3(0.0, 1.0, 4);
	mRotX = 4.6;
	// mRotY = 0;
	mRotY = -0.3;

	update();
}

bool FpsCamera::processEvents(const Uint8 *state)
{
	bool ret = false;

	if(state[SDL_SCANCODE_W])
	{
		cameraPos.z += 0.1*sin(mRotX)*Time::elapsed();
		cameraPos.y += 0.1*sin(mRotY)*Time::elapsed();
		cameraPos.x += 0.1*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_S])
	{
		cameraPos.z -= 0.1*sin(mRotX)*Time::elapsed();
		cameraPos.y -= 0.1*sin(mRotY)*Time::elapsed();
		cameraPos.x -= 0.1*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_A])
	{
		cameraPos.z += 0.1*sin(mRotX-M_PI_2)*Time::elapsed();
		cameraPos.x += 0.1*cos(mRotX-M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_D])
	{
		cameraPos.z += 0.1*sin(mRotX+M_PI_2)*Time::elapsed();
		cameraPos.x += 0.1*cos(mRotX+M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_Z])
	{
		cameraPos.y += 0.1*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_X])
	{
		cameraPos.y -= 0.1*Time::elapsed();
		ret = true;
	}

	if(state[SDL_SCANCODE_LCTRL])
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		if(mCaptureMouse)
		{
			mRotX += 0.01*(x - mLastMX);
			mRotY += 0.01*(mLastMY - y);
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

	mView = glm::lookAt(cameraPos,
						cameraPos + glm::normalize(front),
						glm::vec3(0.0, 1.0, 0.0));
}

// void FpsCamera::applySprite(GLuint uniform_mvp)
// {
// 	glm::mat4 rot(1.0);
//
// 	glm::rotate(rot, cos(-mRotX) * cos(-mRotY), glm::vec3(1,0,0));
// 	glm::rotate(rot, sin(-mRotY), glm::vec3(0,1,0));
// 	glm::rotate(rot, sin(-mRotX) * cos(-mRotY), glm::vec3(0,0,1));
//
// 	glm::mat4 mvp = mProjection * mView * mModel * mPreRot ;
// 	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
// }

