#pragma once
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>

class Shader;
class Texture;

class Sprite
{
public:
	virtual void init(std::string path, std::string shaderName);
	// void init(const char *path, const char *shaderName);
	virtual void setPosition(const glm::mat4 &position);
	virtual void paint();

protected:
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Texture> mTexture;

	GLuint vbo_vertices;
	GLuint vbo_texcoords;
	GLuint ibo_elements;

	GLuint attribute_coord3d;
	GLuint attribute_texcoord;

	glm::mat4 mPosition;
};

