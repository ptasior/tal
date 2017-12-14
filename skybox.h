#pragma once
#include "model.h"

class Skybox
{
public:
	void init(const std::string path);
	void setTexture(const std::string path);
	void load();
	void paint();

private:
	GLuint mVboVert;
	GLuint mVboTex;
	GLuint mIboElem;

	GLuint mAttrVert;
	GLuint mAttrTex;

	glm::mat4 mPosition;

	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Texture> mTexture;
};

