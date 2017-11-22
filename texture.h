#pragma once
#include "gl_header.h"
#include <memory>
#include <mutex>
#include <string>
#include <map>

class Texture
{
public:
	// TODO Add destructor
	~Texture();

	void init(const char *path);
	void apply();

	static std::shared_ptr<Texture> getTexture(const char* path);

private:
	Texture();
	GLuint texture_id;
	GLint uniform_mytexture;

	std::string mName;
	static std::mutex mMutex;
	static std::map<std::string, std::shared_ptr<Texture>> mList;
};

