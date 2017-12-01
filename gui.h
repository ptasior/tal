#pragma once
#include "gl_header.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <selene.h>
#include "log.h"

class GuiSprite;
class Gui;
class Label;
class Box;

class Widget
{
public:
	Widget(std::string texture="");
	~Widget(){Log() << "~";};

	enum LayoutType {ltNone, ltHorizontal, ltVertical};
	void paint();

	void setTop(unsigned int v);
	void setLeft(unsigned int v);
	void setWidth(unsigned int v);
	void setHeight(unsigned int v);

	virtual void setPosition(unsigned int top, unsigned int left);
	virtual void setSize(unsigned int width, unsigned int height);
	virtual void setRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height);

	std::tuple<int, int, int, int> getRect();

	void setLayout(LayoutType t);
	void setColor(int r, int g, int b, int a);

	void addLabel(Label* w);
	void addBox(Box* w);
	void addWidget(Widget* w);
	void addOwnedWidget(std::shared_ptr<Widget> w);

	void removeWidget(Widget* w);

	void onClick(std::function<void(void)> fnc);
	void onClickLua(sel::function<void(void)> fnc);

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

	LayoutType mLayoutType = ltHorizontal;

	// Vector because empty constructor in sel::fuction is deleted and
	// object has to be copied, otherwise will be garbage collected
	// TODO fix me in a more elegant way or at least add asserts(size() <= 1)
	std::vector<sel::function<void(void)>> mOnClickLua;
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
	int getSceneWidth();
	int getSceneHeight();

	Widget& rootWidget();

private:
	std::shared_ptr<Widget> mRoot;
	int mSceneWidth;
	int mSceneHeight;

	GLuint mUniformMVP;
	glm::mat4 mMvp;
};

 
