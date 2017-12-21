#pragma once
#include "sprite.h"


class GuiSprite : public Sprite
{
public:
	void init(const char *path, const char *shaderName);
	void setColor(int r, int g, int b, int a);
	void paint();

private:
	glm::vec4 mColor = glm::vec4(1.0);
};

