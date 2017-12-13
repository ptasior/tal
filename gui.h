#pragma once
#include "widget.h"

#include "gl_header.h"
#include <selene.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>

class Button;
class Console;


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

 
