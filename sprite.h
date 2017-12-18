#pragma once
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>

class Shader;
class Texture;

class Sprite
{
public:
	virtual ~Sprite();
	virtual void init(std::string path, std::string shaderName);
	virtual void setPosition(const glm::mat4 &position);
	// Must be called before init()
	virtual void setSize(double w, double h);
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
	glm::mat4 mRot;

	float mWidth=1;
	float mHeight=1;
};

