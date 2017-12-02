#include "model.h"
#include "shader.h"
#include "texture.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Model::init()
{
	mShader = Shader::getShader("triangle");

	attribute_coord3d = mShader->mkAttrib("coord3d");
	attribute_texcoord = mShader->mkAttrib("texcoord");
	uniform_position = mShader->mkUniform("position");

	load();
}

void Model::setPosition(const glm::mat4 &position)
{
	mPosition = position;
}

void Model::paint()
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

