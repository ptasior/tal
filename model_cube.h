#pragma once
#include "model.h"

class ModelCube : public Model
{
public:
	void init(const std::string& path);
	void load();
	void paint();

private:
	GLuint vbo_cube_vertices;
	GLuint vbo_cube_texcoords;
	GLuint ibo_cube_elements;

	std::shared_ptr<Texture> mTexture;
};

