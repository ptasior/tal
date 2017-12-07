#include "camera.h"
#include "log.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera()
{
}

void Camera::setModel(const glm::mat4& v)
{
	mModel = v;
}

void Camera::setView(const glm::mat4& v)
{
	mView = v;
}

void Camera::setProjection(const glm::mat4& v)
{
	mProjection = v;
}

void Camera::setPreRot(const glm::mat4& v)
{
	mPreRot = v;
}

void Camera::setPostRot(const glm::mat4& v)
{
	mPostRot = v;
}

glm::mat4 Camera::getModel()
{
	return mModel;
}

glm::mat4 Camera::getView()
{
	return mView;
}

glm::mat4 Camera::getProjection()
{
	return mProjection;
}

glm::mat4 Camera::getPreRot()
{
	return mPreRot;
}

glm::mat4 Camera::getPostRot()
{
	return mPostRot;
}


void Camera::init()
{
	Log() << "Camera: init";
	setView(glm::lookAt(glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.0, 0.0, -1.0),
				glm::vec3(0.0, 1.0, 0.0))
			);
	// setModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -2.0)));
	//
	// mCamera->setProjection(glm::perspective(glm::radians(45.0f),
	// 			1.0f * mSceneWidth / mSceneHeight, 0.1f, 10.0f)
	// 		);
}

void Camera::setSceneSize(int w, int h)
{
	mSceneWidth = w;
	mSceneHeight = h;

	setProjection(glm::perspective(glm::radians(45.0f),
				1.0f * mSceneWidth / mSceneHeight, 0.1f, 10.0f)
			);
}

void Camera::apply(GLuint uniform_mvp)
{
	glm::mat4 mvp = mPostRot * mProjection * mView * mModel * mPreRot;
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void Camera::applySprite(GLuint uniform_mvp)
{
	apply(uniform_mvp);
}

bool Camera::processEvents(const Uint8 *state)
{
	return false;
}
