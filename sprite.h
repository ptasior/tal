#pragma once
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>
#include <string>

class Shader;
class Texture;

class Sprite
{
public:
	virtual ~Sprite();
	virtual std::string type();
	virtual void init(std::string path, std::string shaderName);
	virtual void setPosition(const glm::mat4 &position);
	virtual void setShader(const std::string &name);
	virtual void setTexture(const std::string &path);
	virtual void setTextureRepeat(unsigned int x, unsigned int y);
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

	float mWidth=1;
	float mHeight=1;
	unsigned int mRepeatX=1;
	unsigned int mRepeatY=1;
	std::string mPath;
};

