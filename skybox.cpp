#include "skybox.h"
#include "shader.h"
#include "texture.h"
#include "log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Skybox::init(const std::string path)
{
	mShader = Shader::getShader("triangle");

	mAttrVert = mShader->attrib("coord3d");
	mAttrTex = mShader->attrib("texcoord");

	mPosition = glm::mat4(1.0);
	mShader->setUniform("position", {glm::value_ptr(mPosition)});

	const int val = 50.0;
	GLfloat cube_vertices[] = {
		// front
		-val, -val,  val,
		 val, -val,  val,
		 val,  val,  val,
		-val,  val,  val,
		// top
		-val,  val,  val,
		 val,  val,  val,
		 val,  val, -val,
		-val,  val, -val,
		// back
		 val, -val, -val,
		-val, -val, -val,
		-val,  val, -val,
		 val,  val, -val,
		// bottom
		-val, -val, -val,
		 val, -val, -val,
		 val, -val,  val,
		-val, -val,  val,
		// left
		-val, -val, -val,
		-val, -val,  val,
		-val,  val,  val,
		-val,  val, -val,
		// right
		 val, -val,  val,
		 val, -val, -val,
		 val,  val, -val,
		 val,  val,  val,
	};

	GLfloat cube_texcoords[2*4*6] = {
		0.25, 0.5,
		0.5, 0.5,
		0.5, 0.25,
		0.25, 0.25,

		0.25, 0.25,
		0.5, 0.25,
		0.5, 0.0,
		0.25, 0.0,

		0.75, 0.5,
		1, 0.5,
		1, 0.25,
		0.75, 0.25,

		0.25, 0.75,
		0.5, 0.75,
		0.5, 0.5,
		0.25, 0.5,

		0, 0.5,
		0.25, 0.5,
		0.25, 0.25,
		0, 0.25,

		0.5, 0.5,
		0.75, 0.5,
		0.75, 0.25,
		0.5, 0.25
	};
	// for (int i = 1; i < 6; i++)
	// 	memcpy(&cube_texcoords[i*4*2], &cube_texcoords[0], 2*4*sizeof(GLfloat));

	GLushort cube_elements[] = {
		// front
		0,  1,  2,
		2,  3,  0,
		// top
		4,  5,  6,
		6,  7,  4,
		// back
		8,  9, 10,
		10, 11,  8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20,
	};


	glGenBuffers(1, &mVboVert);
	glBindBuffer(GL_ARRAY_BUFFER, mVboVert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mVboTex);
	glBindBuffer(GL_ARRAY_BUFFER, mVboTex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);

	glGenBuffers(1, &mIboElem);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIboElem);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	mTexture = Texture::getTexture(path.c_str());
	mTexture->setClamp();
}

void Skybox::setTexture(const std::string path)
{
	mTexture = Texture::getTexture(path.c_str());
	mTexture->setClamp();
}

void Skybox::paint()
{
	if(!mTexture) return;

	mShader->use();

	mShader->setUniform("position", {glm::value_ptr(mPosition)});

	mTexture->apply();

	glDepthMask(0);

	glEnableVertexAttribArray(mAttrVert);
	glBindBuffer(GL_ARRAY_BUFFER, mVboVert);

	glVertexAttribPointer(
		mAttrVert, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(mAttrTex);
	glBindBuffer(GL_ARRAY_BUFFER, mVboTex);
	glVertexAttribPointer(
		mAttrTex, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIboElem);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(mAttrTex);
	glDisableVertexAttribArray(mAttrVert);

	glDepthMask(1);
}

