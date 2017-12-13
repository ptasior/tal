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
class Button;
class Console;

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
	virtual int getLeft();
	virtual int getTop();

	virtual void setLayout(int t);
	virtual void setColor(int r, int g, int b, int a);
	virtual void setCenter(bool c);
	virtual void setVisible(bool v);
	virtual bool isVisible();

	template<class T>
	void addWidget(T* w);
	template<class T>
	void removeWidget(T* w);
	virtual void addForeignWidget(Widget* w);
	virtual void removeForeignWidget(Widget* w);
	virtual void addOwnedWidget(std::shared_ptr<Widget> w);


	virtual void onClick(std::function<void(void)> fnc);
	virtual void onClickLua(sel::function<void(void)> fnc);

	virtual void focus();
	virtual void unfocus();

protected:
	virtual void setupChild(Widget *w, int pos);
	virtual void setupChildren();
	virtual void updatePosition();
	virtual bool click(int x, int y);
	virtual bool drag(int x, int y);
	virtual bool drop(int x, int y);
	bool isPositionOver(int x, int y);

	virtual void doFocus();
	virtual void doUnfocus();
	virtual bool onDrag(int x, int y);
	virtual bool onDrop(int x, int y);

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
	Label(std::string text);
	void setText(const char *text);
};


class Edit : public Label
{
public:
	Edit(std::string text);
	std::string getText();
	void addText(std::string t);
	void setText(std::string t);

	void focus();
	void unfocus();
	void setOnReturn(std::function<void(void)> fnc);

private:
	std::string mText;
	std::function<void(void)> mOnReturn;
	int mColorFocused = 200;
	int mColorUnfocused = 180;

	friend Console;
};



class Box : public Widget
{
public:
	Box(std::string title);
	virtual void setupChildren();

	virtual void removeForeignWidget(Widget* w);
	virtual void addForeignWidget(Widget* w);
	virtual void addOwnedWidget(std::shared_ptr<Widget> w);

protected:
	std::shared_ptr<Label> mLabel;
	std::shared_ptr<Widget> mContent;
};


class ButtonBox : public Box
{
public:
	ButtonBox(std::string title);
	void setupChildren();

	void addBottomButton(std::shared_ptr<Widget> w);
	void addForeignBottomButton(Button* w);

private:
	std::shared_ptr<Widget> mButtonWidget;
};


class Console : public Widget
{
public:
	Console();
	void log(std::string &log);
	void execute(std::string &cmd);

private:
	std::vector<std::shared_ptr<Label>> mLabels;
	std::vector<std::string> mLines;
	std::shared_ptr<Edit> mEdit;
	const unsigned int LINESCNT = 19;
};


class Button : public Widget
{
public:
	Button(std::string label);
};



class Gui
{
public:
	void init();
	void paint();
	void click(int x, int y);
	void drag(int x, int y);
	void drop(int x, int y);

	void setSceneSize(int w, int h);
	int getSceneWidth();
	int getSceneHeight();
	bool textInput(std::string t);

	bool grabsFocus();

	Widget& rootWidget();
	Console* getConsole();

private:
	std::shared_ptr<Widget> mRoot;
	int mSceneWidth;
	int mSceneHeight;

	glm::mat4 mMvp;
	static Widget *mFocused;
	std::shared_ptr<Console> mConsole;

	friend Widget;
};

#include "gui.hpp"

 
