#include "fps_camera.h"
#include "log.h"
#include "time.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void FpsCamera::init()
{
	Camera::init();

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
	const float speed = 0.2;

	if(state[SDL_SCANCODE_W])
	{
		cameraPos.z += speed*sin(mRotX)*Time::elapsed();
		cameraPos.y += speed*sin(mRotY)*Time::elapsed();
		cameraPos.x += speed*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_S])
	{
		cameraPos.z -= speed*sin(mRotX)*Time::elapsed();
		cameraPos.y -= speed*sin(mRotY)*Time::elapsed();
		cameraPos.x -= speed*cos(mRotX)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_A])
	{
		cameraPos.z += speed*sin(mRotX-M_PI_2)*Time::elapsed();
		cameraPos.x += speed*cos(mRotX-M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_D])
	{
		cameraPos.z += speed*sin(mRotX+M_PI_2)*Time::elapsed();
		cameraPos.x += speed*cos(mRotX+M_PI_2)*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_Z])
	{
		cameraPos.y += speed*Time::elapsed();
		ret = true;
	}
	if(state[SDL_SCANCODE_X])
	{
		cameraPos.y -= speed*Time::elapsed();
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

	mView = glm::lookAt(cameraPos,
						cameraPos + glm::normalize(front),
						glm::vec3(0.0, 1.0, 0.0));
}

void FpsCamera::applySprite(GLuint uniform_mvp)
{
	// glm::mat4 rot(1.0);

	// glm::rotate(rot, cos(-mRotX) * cos(-mRotY), glm::vec3(1,0,0));
	// glm::rotate(rot, sin(-mRotY), glm::vec3(0,1,0));
	// glm::rotate(rot, sin(-mRotX) * cos(-mRotY), glm::vec3(0,0,1));

    // glm::vec3 front(cos(mRotX) * cos(mRotY),
					// sin(mRotY),
					// sin(mRotX) * cos(mRotY));


	// glm::vec3 front(cos(mRotX) * cos(mRotY),
	// 				sin(mRotY),
	// 				sin(mRotX) * cos(mRotY));

	// glm::vec3 front(-1.0);

	// glm::mat4 view = glm::lookAt(cameraPos,
	// 					cameraPos + glm::normalize(front),
	// 					glm::vec3(0.0, 1.0, 0.0));

	// glm::mat4 mvp = mProjection * view * mModel * mPreRot ;
	// glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glm::mat4 mvp = mPostRot * mProjection * mView * mModel * mPreRot;
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

