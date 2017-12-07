#pragma once
#include <map>
#include "gl_header.h"
#include <memory>
#include <mutex>
#include <functional>
#include <string>

class Shader
{
public:
	void init(const char *name);

	GLuint mkAttrib(const char *name);
	GLuint mkUniform(const char *name);
	GLuint var(const char *name);

	void use();
	void setOnChange(std::function<void(void)> fnc);

	static std::shared_ptr<Shader> getShader(const char *name);

private:
	Shader();

	GLuint mProgram;
	std::string mName;
	std::map<std::string, GLuint> mVars;

	static std::mutex mMutex;
	static std::map<std::string, std::shared_ptr<Shader>> mList;
	static std::string mCurrent;
	std::function<void(void)> mOnChange;

	GLuint loadShader(const char * file, GLenum type);
	std::string readFile(const char* filename);
	std::string getGlLog(GLuint object);
};

