#include "camera.h"

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

void Camera::setUniformMVP(GLuint uniform_mvp)
{
	mUniformMVP = uniform_mvp;
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

void Camera::apply()
{
	glm::mat4 mvp = mPostRot * mProjection * mView * mModel * mPreRot;
	glUniformMatrix4fv(mUniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
}

