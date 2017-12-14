#pragma once
#include "gl_header.h"
#include <memory>
#include <mutex>
#include <string>
#include <map>

class SDL_Surface;
class Shader;

class Texture
{
public:
	// TODO Add destructor
	~Texture();

	// When repeat == false, clamping is enabled
	void init(const char *path, Shader *shader);
	void apply();

	void setClamp();
	void setRepeat();

	static std::shared_ptr<Texture> getTexture(const char* path, Shader *s);
	static void unbind();

	// Input surface is deleted
	static SDL_Surface* flip(SDL_Surface *surface, int flags);

private:
	Texture();
	GLuint mTextureId;
	GLint mUniformTexture;

	std::string mName;
	static std::mutex mMutex;
	static std::map<std::string, std::shared_ptr<Texture>> mList;
	Shader *mShader;
};

