#pragma once
#include "gl_header.h"
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	virtual void init();

	void setModel(const glm::mat4& v);
	void setView(const glm::mat4& v);
	void setProjection(const glm::mat4& v);
	void setPreRot(const glm::mat4& v);
	void setPostRot(const glm::mat4& v);

	glm::mat4 getModel();
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getPreRot();
	glm::mat4 getPostRot();

	void setSceneSize(int w, int h);
	virtual void apply(GLuint uniform_mvp);
	virtual void applySprite(GLuint uniform_mvp);
	virtual bool processEvents(const Uint8 *state);

protected:
	glm::mat4 mModel;
	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::mat4 mPreRot;
	glm::mat4 mPostRot;

	int mSceneWidth;
	int mSceneHeight;
};

