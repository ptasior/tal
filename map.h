#pragma once
#include <glm/glm.hpp>
#include "gl_header.h"
#include <memory>
#include <vector>
#include <map>

class Shader;
class Texture;
struct SDL_Surface;

class Map
{
public:
	~Map();
	virtual void init(const std::string path);
	virtual void addTexture(const std::string path);
	virtual void setRect(double l, double t, double r, double b, double d, double u);
	virtual void setPosition(const glm::mat4 &position);
	virtual void paint();
	virtual double getAltitude(double x, double z);

protected:
	std::shared_ptr<Shader> mShader;
	std::vector<std::shared_ptr<Texture>> mTextures;

	GLuint vboVert;
	GLuint vboTex;
	GLuint vboNorm;
	GLuint iboElements;

	GLuint mAttrVert;
	GLuint mAttrTex;
	GLuint mAttrNorm;

	glm::mat4 mPosition;
	SDL_Surface* mImg;

	float left = -0.5;
	float top = -0.5;
	float right = 0.5;
	float bottom = 0.5;
	float down = 0;
	float up = 0.5;
};

