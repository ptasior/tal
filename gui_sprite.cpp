#include "gui_sprite.h"
#include "log.h"
#include "shader.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void GuiSprite::init(const char *path, const char *shaderName)
{
	Sprite::init(path, shaderName);
	mColor = glm::vec4(1.0);
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
}

void GuiSprite::setColor(int r, int g, int b, int a)
{
	mColor.r = r/255.0f;
	mColor.g = g/255.0f;
	mColor.b = b/255.0f;
	mColor.a = a/255.0f;
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
}

void GuiSprite::paint()
{
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
	Sprite::paint();
}

