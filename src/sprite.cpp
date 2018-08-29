#include "sprite.h"
#include "log.h"
#include "shader.h"
#include "renderer.h"
#include "texture.h"
#include "global.h"
#include "config.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sprite::~Sprite()
{
	// if(mTexture)
	// 	Log() << "~Sprite" << mTexture->getName();
	// else
	// 	Log() << "~Sprite no texture";

	if(!mShader) return;

	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
}

void Sprite::init(std::string path, std::string shaderName)
{
	mPath = path;

	GLfloat vertices[] = {
		-mWidth, -mHeight,  0.0,
		 mWidth, -mHeight,  0.0,
		 mWidth,  mHeight,  0.0,
		-mWidth,  mHeight,  0.0
	};

	float t=0, l=0, w=mRepeatX, h=mRepeatY;
	if(path.substr(0, 7) == "letter-") // A letter
	{
		const int SIZE = 16;
		char let = path[7];

		t = 1.0 - (1.0*((let/SIZE)+1)) / SIZE;
		l = (1.0*(let%SIZE)) / SIZE;
		w = 1.0 / SIZE;
		h = 1.0 / SIZE;

		const float CROP = 0;//1.0/(SIZE*16);
		t += CROP;
		l += CROP*2;
		w -= CROP*4;
		h -= CROP*2;
	}

	GLfloat texcoords[] = {
		l,   t,
		l+w, t,
		l+w, t+h,
		l,   t+h,
	};

	GLushort elements[] = {
		0,  1,  2,
		2,  3,  0
	};

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	mShader = Global::get<Renderer>()->shader(shaderName.c_str());

	attribute_coord3d = mShader->attrib("coord3d");
	attribute_texcoord = mShader->attrib("texcoord");


	if(path.empty()) return;

	if(path.substr(0, 7) == "letter-") // A letter
		mTexture = Global::get<Renderer>()->texture(Global::get<Config>()->get("fontTexture").c_str(), mShader.get());
	else
		mTexture = Global::get<Renderer>()->texture(path.c_str(), mShader.get());
}

void Sprite::setPosition(const glm::mat4 &position)
{
	mPosition = position;
	mShader->setUniform("position", {glm::value_ptr(mPosition)});
}

void Sprite::setSize(double w, double h)
{
	mWidth = (float)w;
	mHeight = (float)h;
}

void Sprite::paint()
{
	mShader->use();
	mShader->setUniform("position", {glm::value_ptr(mPosition)});

	if(mTexture)
		mTexture->apply();
	else
		Texture::unbind();

	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);

	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);

	glVertexAttribPointer(
		attribute_texcoord, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_texcoord);
	glDisableVertexAttribArray(attribute_coord3d);
}

void Sprite::setShader(const std::string &name)
{
	mShader = Global::get<Renderer>()->shader(name.c_str());
}

void Sprite::setTexture(const std::string &path)
{
	if(!mShader) return;

	mTexture = Global::get<Renderer>()->texture(path.c_str(), mShader.get());
}

void Sprite::setTextureRepeat(unsigned int x, unsigned int y)
{
	mRepeatX = x;
	mRepeatY = y;

	float t=0, l=0, w=mRepeatX, h=mRepeatY;

	GLfloat texcoords[] = {
		l,   t,
		l+w, t,
		l+w, t+h,
		l,   t+h,
	};

	// TODO Is it necessary to remove and create it again?
	glDeleteBuffers(1, &vbo_texcoords);
	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
}

std::string Sprite::type()
{
	return std::string("Sprite: p:") + mPath + " s:" +mShader->getName();
}

