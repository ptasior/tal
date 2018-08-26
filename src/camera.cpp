#include "camera.h"
#include "shader.h"
#include "log.h"

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

void Camera::setCameraPos(const glm::vec3& v)
{
	mCameraPos = v;
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

glm::vec3 Camera::getCameraPos()
{
	return mCameraPos;
}


void Camera::init()
{
	Log() << "Camera: init";
	// setView(glm::lookAt(glm::vec3(0.0, 0.0, 0.0),
	// 			glm::vec3(0.0, 0.0, -1.0),
	// 			glm::vec3(0.0, 1.0, 0.0))
	// 		);
	// setModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -2.0)));
	//
	// mCamera->setProjection(glm::perspective(glm::radians(45.0f),
	// 			1.0f * mSceneWidth / mSceneHeight, 0.1f, 10.0f)
	// 		);

	// Shader::getShader("map")->setUniform("mvp", Shader::Value{glm::value_ptr(mMvp)});
	// Shader::getShader("model")->setUniform("mvp", Shader::Value{glm::value_ptr(mMvp)});
	// Shader::getShader("triangle")->setUniform("mvp", Shader::Value{glm::value_ptr(mMvp)});
	// Shader::getShader("sprite")->setUniform("mvp", Shader::Value{glm::value_ptr(mMvp)});
}

void Camera::setSceneSize(int w, int h)
{
	mSceneWidth = w;
	mSceneHeight = h;

	setProjection(glm::perspective(glm::radians(45.0f),
				1.0f * mSceneWidth / mSceneHeight, 0.1f, 100.0f)
			);
}

void Camera::apply()
{
	mMvp = mPostRot * mProjection * mView * mModel * mPreRot;
	// glUniformMatrix4fv(uniform_mvp, 1, GL_FALS glm::value_ptr(mvp));
}

bool Camera::processEvents(const Uint8 *state)
{
	return false;
}

bool Camera::onEvent(SDL_Event &event)
{
	return false;
}

void Camera::setupShaderMVP(Shader *shader)
{
	shader->setUniform("mvp", Shader::Value{glm::value_ptr(mMvp)});
}

