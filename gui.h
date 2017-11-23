#pragma once
#include "gl_header.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Sprite;
class Gui;

class Widget
{
public:
	enum LayoutType {ltNone, ltHorizontal, ltVertical};
	void init(Widget* parent, const char *texture);
	void paint();

	void setTop(unsigned int v);
	void setLeft(unsigned int v);
	void setWidth(unsigned int v);
	void setHeight(unsigned int v);

	void setPosition(unsigned int top, unsigned int left);
	void setSize(unsigned int width, unsigned int height);
	void setRect(unsigned int top, unsigned int left, unsigned int width, unsigned int height);

	void setLayout(LayoutType t);
	void addWidget(std::shared_ptr<Widget> w);

private:
	void updatePosition();

	std::vector<std::shared_ptr<Widget>> mWidgets;
	std::shared_ptr<Sprite> mSprite;
	Widget* mParent;

	unsigned int mTop;
	unsigned int mLeft;
	unsigned int mWidth = 10;
	unsigned int mHeight = 10;
	LayoutType mLayoutType = ltNone;
};

class Gui
{
public:
	void init();
	void paint();

	void setSceneSize(int w, int h);

private:
	std::shared_ptr<Widget> mRoot;
	int mSceneWidth;
	int mSceneHeight;

	GLuint mUniformMVP;
	glm::mat4 mMvp;
};

 
