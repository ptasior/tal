#pragma once
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

	glm::mat4 mPosition;
};

