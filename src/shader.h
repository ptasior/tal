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
	Shader();

	union Value
	{
		GLfloat* float_ptr;
		float float_val;
		float float_v4[4];
		float float_v3[3];
		int int_val;
	};

	void init(const char *name);

	void setUniform(const char* name, Value v);
	GLuint attrib(const char* name);
	bool hasUniform(const char* name) const;
	const std::string &getName() const;

	void use();
	void setOnChange(std::function<void(void)> fnc);

private:

	GLuint loadShader(const std::string &file, GLenum type);
	void readVariables();
	std::string getGlLog(GLuint object);
	void useProgram();

	GLuint mProgram;
	std::string mName;

	// Args: variable name, location(id), type(var type), value, isVarSet
	std::map<std::string, std::tuple<GLuint, GLenum, Value, bool>> mUniforms;
	std::map<std::string, std::tuple<GLuint, GLenum>> mAttribs;

	std::function<void(void)> mOnChange;

	static std::string mCurrent;
};

