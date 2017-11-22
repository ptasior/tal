#include "shader.h"
#include "gl_header.h"
#include "log.h"
#include <fstream>

std::map<std::string, std::shared_ptr<Shader>> Shader::mList;
std::string Shader::mCurrent;
std::mutex Shader::mMutex;

Shader::Shader()
{
}

void Shader::init(const char *name)
{
	mName = name;
	std::string file = std::string("shaders/") + name;
	GLuint vertexShader = loadShader((file + ".v.glsl").c_str(), GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader((file + ".f.glsl").c_str(), GL_FRAGMENT_SHADER);

	mProgram = glCreateProgram();

	glAttachShader(mProgram, vertexShader);
	glAttachShader(mProgram, fragmentShader);
	glLinkProgram(mProgram);

	GLint link_ok = GL_FALSE;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
		Log(Log::DIE) << "Error in glLinkProgram in " << name;
}

GLuint Shader::mkAttrib(const char *name)
{
	GLuint attrib = glGetAttribLocation(mProgram, name);
	if(attrib == -1)
		Log(Log::DIE) << "Could not bind attribute " << name;

	mVars[name] = attrib;
	return attrib;
}

GLuint Shader::mkUniform(const char *name)
{
	GLuint uniform = glGetUniformLocation(mProgram, name);
	if(uniform == -1)
		Log(Log::DIE) << "Could not bind uniform " << name;

	mVars[name] = uniform;
	return uniform;
}

GLuint Shader::loadShader(const char * file, GLenum type)
{
	GLuint shader = glCreateShader(type);

	std::string lines = readFile(file);
	auto ptr = lines.c_str();

	const char* precision =
		"#ifdef GL_ES                        \n"
		"#  ifdef GL_FRAGMENT_PRECISION_HIGH \n"
		"     precision highp float;         \n"
		"#  else                             \n"
		"     precision mediump float;       \n"
		"#  endif                            \n"
		"#endif                              \n";

	const GLchar* sources[] = {
		precision,
		ptr
	}; // When adding update sizeof below

	glShaderSource(shader, 2, sources, NULL);
	glCompileShader(shader);

	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

	if(!compile_ok)
		Log(Log::DIE) << "Error in vertex shader\n" << getGlLog(shader);

	return shader;
}


std::string Shader::readFile(const char* filename)
{
	std::ifstream t(filename);
	if(t.fail())
		Log(Log::DIE) << "Cannot open " << filename;

	return std::string((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());
}


std::string Shader::getGlLog(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else
		return "getGlLog: Not a shader or a program";

	std::string ret;
	ret.resize(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, &ret[0]);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, &ret[0]);
	
	return ret;
}

GLuint Shader::var(const char *name)
{
	return mVars[name];
}

void Shader::use()
{
	std::lock_guard<std::mutex> lock(mMutex);

	if(mCurrent == mName) return; // Currently used
	mCurrent = mName;
	glUseProgram(mProgram);
}

std::shared_ptr<Shader> Shader::getShader(const char *name)
{
	std::lock_guard<std::mutex> lock(mMutex);

	if(!mList.count(name)) // Not yet initiaalised
	{
		// mList[name] = std::make_shared<Shader>();
		mList[name].reset(new Shader);
		mList[name]->init(name);
	}

	return mList[name];
}

