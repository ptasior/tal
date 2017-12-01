#pragma once
#define GLM_FORCE_RADIANS
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Model
{
public:
	void init()
	{
		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// top
			-1.0,  1.0,  1.0,
			 1.0,  1.0,  1.0,
			 1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
			// back
			 1.0, -1.0, -1.0,
			-1.0, -1.0, -1.0,
			-1.0,  1.0, -1.0,
			 1.0,  1.0, -1.0,
			// bottom
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0, -1.0,  1.0,
			-1.0, -1.0,  1.0,
			// left
			-1.0, -1.0, -1.0,
			-1.0, -1.0,  1.0,
			-1.0,  1.0,  1.0,
			-1.0,  1.0, -1.0,
			// right
			 1.0, -1.0,  1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
			 1.0,  1.0,  1.0,
		};

		GLfloat cube_texcoords[2*4*6] = {
			// front
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,
		};
		for (int i = 1; i < 6; i++)
			memcpy(&cube_texcoords[i*4*2], &cube_texcoords[0], 2*4*sizeof(GLfloat));

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


		glGenBuffers(1, &vbo_cube_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &vbo_cube_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);

		glGenBuffers(1, &ibo_cube_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);


		mShader = Shader::getShader("triangle");

		attribute_coord3d = mShader->mkAttrib("coord3d");
		attribute_texcoord = mShader->mkAttrib("texcoord");
		uniform_position = mShader->mkUniform("position");

		mTexture = Texture::getTexture("assets/tex.png");
	}

	void setPosition(const glm::mat4 &position)
	{
		mPosition = position;
	}

	void paint()
	{
		// mShader->use();
		//
		glUniformMatrix4fv(uniform_position, 1, GL_FALSE, glm::value_ptr(mPosition));

		mTexture->apply();

		glEnableVertexAttribArray(attribute_coord3d);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

		glVertexAttribPointer(
			attribute_coord3d, // attribute
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
		);

		glEnableVertexAttribArray(attribute_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
		glVertexAttribPointer(
			attribute_texcoord, // attribute
			2,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);


		// Draw a triangle from the 3 vertices
		// glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(attribute_texcoord);
		glDisableVertexAttribArray(attribute_coord3d);
	}
private:
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Texture> mTexture;

	GLuint vbo_cube_vertices;
	GLuint vbo_cube_texcoords;
	GLuint ibo_cube_elements;

	GLuint attribute_coord3d;
	GLuint attribute_texcoord;
	GLint uniform_position;

	glm::mat4 mPosition;
};

