#include "gui_sprite.h"
#include "log.h"

#define GLM_FORCE_RADIANS
#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void GuiSprite::init(const char *path, const char *shaderName)
{
	Sprite::init(path, shaderName);
	attribute_color = mShader->mkUniform("color");
}

void GuiSprite::setColor(int r, int g, int b, int a)
{
	mColor.r = r/255.0f;
	mColor.g = g/255.0f;
	mColor.b = b/255.0f;
	mColor.a = a/255.0f;
}

void GuiSprite::paint()
{
	glUniform4f(attribute_color, mColor.r, mColor.g, mColor.b, mColor.a);
	Sprite::paint();
}

