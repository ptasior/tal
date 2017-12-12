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

	enum LayoutType {ltNone, ltHorizontal, ltVertical};
	enum OverflowPolicy {opNone, opResize, opClip};
	virtual void paint();

	virtual void setTop(unsigned int v);
	virtual void setLeft(unsigned int v);
	virtual void setWidth(unsigned int v);
	virtual void setHeight(unsigned int v);
	virtual void setPadding(unsigned int h, unsigned int v);

	virtual void setPosition(unsigned int top, unsigned int left);
	virtual void setSize(unsigned int width, unsigned int height);
	virtual void setRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height);
	virtual void setOverflow(int p);

	virtual std::tuple<int, int, int, int> getRect();

	virtual void setLayout(int t);
	virtual void setColor(int r, int g, int b, int a);
	virtual void setVisible(bool v);
	virtual void setCenter(bool c);

	template<class T>
	void addWidget(T* w);
	template<class T>
	void removeWidget(T* w);
	virtual void addForeignWidget(Widget* w);
	virtual void removeForeignWidget(Widget* w);
	virtual void addOwnedWidget(std::shared_ptr<Widget> w);


	virtual void onClick(std::function<void(void)> fnc);
	virtual void onClickLua(sel::function<void(void)> fnc);

protected:
	virtual void setupChild(Widget *w, int pos);
	virtual void setupChildren();
	virtual void updatePosition();
	virtual bool click(int x, int y);

	// Widgets created in Lua
	std::vector<Widget*> mForeignWidgets;
	std::vector<std::shared_ptr<Widget>> mWidgets;
	std::shared_ptr<GuiSprite> mSprite;
	Widget* mParent;

	unsigned int mTopOffset = 0;
	unsigned int mLeftOffset = 0;
	unsigned int mTop = 0;
	unsigned int mLeft = 0;
	unsigned int mWidth = 0;
	unsigned int mHeight = 0;
	unsigned int mPaddingVert = 5;
	unsigned int mPaddingHoris = 5;
	unsigned int mSpacing = 2;
	bool mVisible = true;
	bool mCenter = false;

	LayoutType mLayoutType = ltHorizontal;
	OverflowPolicy mOverflow = opNone;

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
	Label(std::string &text);
	void setText(const char *text);
};



class Box : public Widget
{
public:
	Box(std::string &title);
	void setupChildren();

	void removeForeignWidget(Widget* w);
	void addForeignWidget(Widget* w);
	void addOwnedWidget(std::shared_ptr<Widget> w);

private:
	std::shared_ptr<Label> mLabel;
	std::shared_ptr<Widget> mContent;
};



class Button : public Widget
{
public:
	Button(std::string &label);
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

	glm::mat4 mMvp;
};

#include "gui.hpp"

 
