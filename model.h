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
	virtual void init(const std::string& path);
	virtual void setPosition(const glm::mat4 &position);
	virtual void paint();

protected:
	virtual void load(const std::string& path) = 0;

	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Texture> mTexture;

	GLuint vbo_cube_vertices;
	GLuint vbo_cube_texcoords;
	GLuint ibo_cube_elements;

	GLuint attribute_coord3d;
	GLuint attribute_texcoord;
	GLuint attribute_vnorm;
	GLint uniform_position;

	glm::mat4 mPosition;
};

