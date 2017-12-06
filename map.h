#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>

class Shader;
class Texture;

class Map
{
public:
	virtual void init(const std::string& path);
	virtual void setPosition(const glm::mat4 &position);
	virtual void paint();

protected:
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Texture> mTexture;

	GLuint vboVert;
	GLuint vboTex;
	GLuint vboVNorm;
	GLuint iboElements;

	GLuint attribute_coord3d;
	GLuint attribute_texcoord;
	GLuint attribute_vnorm;
	GLint uniform_position;

	glm::mat4 mPosition;
};

