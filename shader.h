#pragma once
#include <map>
#include "gl_header.h"

class Shader
{
public:
	Shader();

	void init(const char *name);

	GLuint mkAttrib(const char *name);
	GLuint mkUniform(const char *name);
	GLuint var(const char *name);

	void use();

private:
	GLuint mProgram;
	std::map<std::string, GLuint> mVars;

	GLuint loadShader(const char * file, GLenum type);
	std::string readFile(const char* filename);
	std::string getGlLog(GLuint object);
};

