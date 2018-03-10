#include "sprite_gui.h"
#include "log.h"
#include "shader.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void GuiSprite::init(const char *path, const char *shaderName)
{
	Sprite::init(path, shaderName);
	mColor = glm::vec4(1.0); // White by default
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
	mColorIsSet =false;
}

void GuiSprite::setColor(int r, int g, int b, int a)
{
	mColorIsSet = true;
	mColor.r = r/255.0f;
	mColor.g = g/255.0f;
	mColor.b = b/255.0f;
	mColor.a = a/255.0f;
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
	mShader->setUniform("colorOnly", Shader::Value{.int_val = !mTexture});
}

void GuiSprite::paint()
{
	if(!mColorIsSet && !mTexture) return;

	mShader->use();
	mShader->setUniform("color", Shader::Value{.float_v4 = {mColor.r, mColor.g, mColor.b, mColor.a}});
	mShader->setUniform("colorOnly", Shader::Value{.int_val = !mTexture});
	Sprite::paint();
}

