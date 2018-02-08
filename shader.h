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
	const std::string &getName() const;

	void use();
	void setOnChange(std::function<void(void)> fnc);

	static std::shared_ptr<Shader> getShader(const char *name);

private:
	Shader();

	GLuint loadShader(const char * file, GLenum type);
	void readVariables();
	std::string readFile(const char* filename);
	std::string getGlLog(GLuint object);
	void useProgram();

	GLuint mProgram;
	std::string mName;

	// Args: variable name, location(id), type(var type), value, isVarSet
	std::map<std::string, std::tuple<GLuint, GLenum, Value, bool>> mUniforms;
	std::map<std::string, std::tuple<GLuint, GLenum>> mAttribs;

	std::function<void(void)> mOnChange;

	static std::mutex mMutex;
	static std::map<std::string, std::shared_ptr<Shader>> mList;
	static std::string mCurrent;
};

