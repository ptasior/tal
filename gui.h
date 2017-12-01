#pragma once
#include "gl_header.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>

class GuiSprite;
class Gui;
class Label;
class Box;

class Widget
{
public:
	Widget(const char *texture=nullptr);

	enum LayoutType {ltNone, ltHorizontal, ltVertical};
	void paint();

	void setTop(unsigned int v);
	void setLeft(unsigned int v);
	void setWidth(unsigned int v);
	void setHeight(unsigned int v);

	virtual void setPosition(unsigned int top, unsigned int left);
	virtual void setSize(unsigned int width, unsigned int height);
	virtual void setRect(unsigned int top, unsigned int left, unsigned int width, unsigned int height);

	std::tuple<int, int, int, int> getRect();

	void setLayout(LayoutType t);

	void addLabel(Label* w);
	void addBox(Box* w);
	void addWidget(std::shared_ptr<Widget> w);

	void onClick(std::function<void(void)> fnc);

protected:
	void setupChild(Widget *w);
	void updatePosition();
	bool click(int x, int y);

	// Widgets created in Lua
	std::vector<Widget*> mForeignWidgets;
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
	std::function<void(void)> mOnClick;

	friend Gui;
};



class Label : public Widget
{
public:
	Label(std::string text);
	void setText(const char *text);
};



class Box : public Widget
{
public:
	Box();
};



class Gui
{
public:
	void init();
	void paint();
	void click(int x, int y);

	void setSceneSize(int w, int h);

	Widget& rootWidget();

private:
	std::shared_ptr<Widget> mRoot;
	int mSceneWidth;
	int mSceneHeight;

	GLuint mUniformMVP;
	glm::mat4 mMvp;
};

 
