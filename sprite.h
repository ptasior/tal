#pragma once
#define GLM_FORCE_RADIANS
#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sprite
{
public:
	void init(const char *path)
	{
		GLfloat vertices[] = {
			-1.0, -1.0,  0.0,
			 1.0, -1.0,  0.0,
			 1.0,  1.0,  0.0,
			-1.0,  1.0,  0.0
		};

		GLfloat texcoords[] = {
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,
		};

		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

		mShader = Shader::getShader("triangle");
		attribute_coord3d = mShader->mkAttrib("coord3d");
		attribute_texcoord = mShader->mkAttrib("texcoord");
		uniform_position = mShader->mkUniform("position");
		uniform_mvp = mShader->mkUniform("mvp");

		mTexture.init(path);
	}

	void setMVP(glm::mat4 &mvp)
	{
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	void setPosition(glm::mat4 &position)
	{
		glUniformMatrix4fv(uniform_position, 1, GL_FALSE, glm::value_ptr(position));
	}

	void paint()
	{
		mShader->use();
		mTexture.apply();

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

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(attribute_texcoord);
		glDisableVertexAttribArray(attribute_coord3d);
	}

private:
	std::shared_ptr<Shader> mShader;

	GLuint vbo_vertices;
	GLuint vbo_texcoords;

	GLuint attribute_coord3d;
	GLuint attribute_texcoord;
	GLint uniform_mvp;
	GLint uniform_position;

	Texture mTexture;
};

