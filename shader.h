#pragma once
#include <map>
#include "gl_header.h"
#include <memory>
#include <mutex>

class Shader
{
public:
	void init(const char *name);

	GLuint mkAttrib(const char *name);
	GLuint mkUniform(const char *name);
	GLuint var(const char *name);

	void use();
	static std::shared_ptr<Shader> getShader(const char *name);

private:
	Shader();

	GLuint mProgram;
	std::string mName;
	std::map<std::string, GLuint> mVars;

	static std::mutex mMutex;
	static std::map<std::string, std::shared_ptr<Shader>> mList;
	static std::string mCurrent;

	GLuint loadShader(const char * file, GLenum type);
	std::string readFile(const char* filename);
	std::string getGlLog(GLuint object);
};

