#pragma once
#include "gl_header.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class GuiSprite;
class Gui;

class Widget
{
public:
	Widget(Widget* parent=nullptr, const char *texture=nullptr);

	enum LayoutType {ltNone, ltHorizontal, ltVertical};
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

protected:
	void updatePosition();

	std::vector<std::shared_ptr<Widget>> mWidgets;
	std::shared_ptr<GuiSprite> mSprite;
	Widget* mParent;

	unsigned int mTopOffset = 0;
	unsigned int mLeftOffset = 0;
	unsigned int mTop = 0;
	unsigned int mLeft = 0;
	unsigned int mWidth = 10;
	unsigned int mHeight = 10;
	unsigned int mPaddingVert = 5;
	unsigned int mPaddingHoris = 5;
	unsigned int mSpacing = 2;

	LayoutType mLayoutType = ltNone;
};


class Label : public Widget
{
public:
	Label(Widget *parent, const char *text);
	void setText(const char *text);
};

class Box : public Widget
{
public:
	Box(Widget *parent);
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

 
