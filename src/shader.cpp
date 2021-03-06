#include "shader.h"
#include "game.h"
#include "renderer.h"
#include "gl_header.h"
#include "log.h"
#include "global.h"
#include "stream_reader.h"
#include <vector>
#include <cassert>

std::string Shader::mCurrent;

const std::map<GLenum, std::function<void(GLint, Shader::Value)>> uniformFunctions =
{
	{GL_BOOL, [](GLint id, Shader::Value v){glUniform1i(id, v.int_val);} },
	{GL_INT, [](GLint id, Shader::Value v){glUniform1i(id, v.int_val);} },
	{GL_SAMPLER_2D, [](GLint id, Shader::Value v){glUniform1i(id, v.int_val);} },
	{GL_FLOAT_MAT4, [](GLint id, Shader::Value v){glUniformMatrix4fv(id, 1, GL_FALSE, v.float_ptr);} },
	{GL_FLOAT_VEC4, [](GLint id, Shader::Value v){glUniform4f(id, v.float_v4[0], v.float_v4[1], v.float_v4[2], v.float_v4[3]);} }
};

Shader::Shader()
{
}

void Shader::init(const char *name)
{
	Log() << "Shader: loading " << name;
	mName = name;
	std::string file = std::string("shaders/") + name;
	if(!Global::get<Game>()->hasResource(file + ".v.glsl"))
	{
		file = std::string("defaults/")+file;
		if(!Global::get<Game>()->hasResource(file + ".v.glsl"))
			Log(Log::DIE) << "Shader: Shader does not exist: " << name;
	}

	GLuint vertexShader = loadShader(file + ".v.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader(file + ".f.glsl", GL_FRAGMENT_SHADER);

	mProgram = glCreateProgram();

	glAttachShader(mProgram, vertexShader);
	glAttachShader(mProgram, fragmentShader);
	glLinkProgram(mProgram);

	GLint link_ok = GL_FALSE;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &link_ok);

	if(!link_ok)
		Log(Log::DIE) << "Shader: Error in glLinkProgram in " << name;

	glDetachShader(mProgram, vertexShader);
	glDetachShader(mProgram, fragmentShader);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	readVariables();
}

void Shader::readVariables()
{
	GLint maxUniformNameLen, noOfUniforms;
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLen);
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, &noOfUniforms);

	GLint maxAttribNameLen, noOfAttributes;
	glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLen);
	glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTES, &noOfAttributes);

	GLint read, size;
	GLenum type;

	std::vector<GLchar>unifN(maxUniformNameLen, 0);
	for(GLint i = 0; i < noOfUniforms; ++ i)
	{
		glGetActiveUniform(mProgram, i, maxUniformNameLen, &read, &size, &type, unifN.data());
		mUniforms[unifN.data()] = std::make_tuple(
				glGetUniformLocation(mProgram, unifN.data()), type, Value{nullptr}, false
			);
	}

	std::vector<GLchar>attrN(maxAttribNameLen, 0);
	for(GLint i = 0; i < noOfAttributes; ++ i)
	{
		glGetActiveAttrib(mProgram, i, maxAttribNameLen, &read, &size, &type, attrN.data());
		mAttribs[attrN.data()] = std::make_tuple(glGetAttribLocation(mProgram, attrN.data()), type);
	}

#ifndef NDEBUG
	for(auto k : mAttribs)
		Log() << "  attrib " <<  std::get<0>(k);

	for(auto k : mUniforms)
		Log() << "  uniform " <<  std::get<0>(k);
#endif
}

GLuint Shader::loadShader(const std::string &file, GLenum type)
{
	GLuint shader = glCreateShader(type);

	auto ptr = Global::get<Game>()->openResource(file)->readToString();

	const char* precision =
	#ifdef DESKTOP
		"#version 130                        \n"
	#endif
		"#ifdef GL_ES                        \n"
		"#  ifdef GL_FRAGMENT_PRECISION_HIGH \n"
		"     precision highp float;         \n"
		"#  else                             \n"
		"     precision mediump float;       \n"
		"#  endif                            \n"
		"#endif                              \n\n\n";

	const GLchar* sources[] = {
		precision,
		ptr.c_str()
	}; // When adding update sizeof below

	glShaderSource(shader, 2, sources, NULL);
	glCompileShader(shader);

	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

	if(!compile_ok)
		Log(Log::DIE) << "Shader: Error in vertex shader "
					  << mName << " ("
					  << file
					  << ")\n" << getGlLog(shader);

	return shader;
}


std::string Shader::getGlLog(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else
		return "Shader: getGlLog: Not a shader or a program";

	std::string ret;
	ret.resize(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, &ret[0]);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, &ret[0]);
	
	return ret;
}


void Shader::useProgram()
{
	glUseProgram(mProgram);
}


void Shader::use()
{
	if(mCurrent == mName) return; // Currently used

	mCurrent = mName;

	useProgram();
	// Log() << "Shader: Using " << mName;

	if(mOnChange) mOnChange();

	// Apply all previous values
	for(auto u : mUniforms)
	{
#ifndef NDEBUG
		if(!std::get<3>(u.second))
			Log(Log::DIE) << "Shader: Value " << u.first
						<< " not inititlised in shader " << mName;
		else
#endif
			uniformFunctions.at(std::get<1>(u.second))(
						std::get<0>(u.second), std::get<2>(u.second)
					);
	}
}


void Shader::setOnChange(std::function<void(void)> fnc)
{
	mOnChange = fnc;
}

void Shader::setUniform(const char* name, Value v)
{
#ifndef NDEBUG
	if(!mUniforms.count(name))
		Log(Log::DIE) << "Shader: No uniform \"" << name << "\" in shader \"" << mName  << "\"";
#endif

	auto attr = mUniforms[name];

#ifndef NDEBUG
	if(!uniformFunctions.count(std::get<1>(attr)))
		Log(Log::DIE) << "Shader: No function defined for given uniform type " << name << " in " << mName;
#endif

	// Log() << "Shader: set " << mName << " " << name << " val " << (long)v.float_ptr;

	// Switch to current shader
	if(mCurrent != mName)
		useProgram();

	auto f = uniformFunctions.at(std::get<1>(attr));
	f(std::get<0>(attr), v);

	std::get<2>(mUniforms[name]) = v;

#ifndef NDEBUG
	std::get<3>(mUniforms[name]) = true;
#endif

	// Restore previous shader
	if(mCurrent != mName && !mCurrent.empty())
		Global::get<Renderer>()->shader(mCurrent.c_str())->useProgram();
}

GLuint Shader::attrib(const char* name)
{
#ifndef NDEBUG
	if(!mAttribs.count(name))
		Log(Log::DIE) << "No attrib " << name << " in " << mName;
#endif

	return std::get<0>(mAttribs[name]);
}

const std::string& Shader::getName() const
{
	return mName;
}


bool Shader::hasUniform(const char* name) const
{
	return mUniforms.count(name);
}
