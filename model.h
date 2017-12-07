#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>

class Shader;
class Texture;

class Model
{
public:
	virtual void init(const std::string path);
	virtual void setPosition(const glm::mat4 &position);
	virtual void paint(); // Lua requires non abstract classes

protected:
	std::shared_ptr<Shader> mShader;

	GLuint mAttrVert;
	GLuint mAttrTex;
	GLuint mAttrNorm;
	GLint mUniformPosition;

	glm::mat4 mPosition;
};

