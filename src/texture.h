#pragma once
#include "gl_header.h"
#include <memory>
#include <string>
#include <map>

struct SDL_Surface;
class Shader;

class Texture
{
public:
	Texture();
	// TODO Add destructor
	virtual ~Texture();

	// When repeat == false, clamping is enabled
	void init(const char *path, Shader *shader);
	void apply();

	void setClamp();
	void setRepeat();

	// For multi texturing
	void setName(std::string name, int id);
	std::string getName();

	static void unbind();

	// Input surface is deleted
	static SDL_Surface* flip(SDL_Surface *surface, int flags);

private:
	GLuint mTextureId;
	GLint mUniformTexture;
	std::string mGlslName;
	int mGlslId;

	std::string mName;
	Shader *mShader;
};

