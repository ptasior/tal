#pragma once
#include "widget.h"

#include "gl_header.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>

class Button;
class Console;
class Label;
class GuiSprite;

class Checkbox : public Widget
{
public:
	Checkbox(bool s);
	void setChecked(bool s);
	bool isChecked();

private:
	void update();

	bool mChecked;
	std::shared_ptr<GuiSprite> mCheckedSprite;
	std::shared_ptr<GuiSprite> mUnCheckedSprite;
};


class Scroll : public Widget
{
public:
	Scroll();

	virtual void addForeignWidget(Widget* w);
	virtual void addOwnedWidget(std::shared_ptr<Widget> w);
	virtual void removeForeignWidget(Widget* w);
	virtual void removeOwnedWidget(Widget* w);
	virtual void clear();

protected:
	virtual void setupChildren();

	void onUp();
	void onDown();

	std::shared_ptr<Button> mButtonUp;
	std::shared_ptr<Button> mButtonDown;
	std::shared_ptr<Widget> mContent;
};



class MultiLine : public Widget
{
public:
	MultiLine(std::string text);
	virtual void setText(std::string text);
	virtual void resize();
	virtual Label* label(int l);
	virtual int linesCount();

private:
	std::vector<std::shared_ptr<Label>> mLabels;
	std::vector<std::string> mLines;
};



class Label : public Widget
{
public:
	Label(std::string text);
	virtual void setText(std::string text);
	virtual const std::string& getText();
	virtual void setTextColor(int r, int g, int b, int a);

	static unsigned int WIDTH;
	static unsigned int HEIGHT;

private:
	glm::vec4 mColor;
	std::string mText;
};



class Edit : public Label
{
public:
	Edit(std::string text);
	virtual void addText(std::string t);
	virtual void setText(std::string t);
	virtual const std::string& getText();

	virtual void focus();
	virtual void unfocus();
	virtual void setOnReturn(std::function<void(void)> fnc);

private:
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
	virtual void setTitle(std::string title);

	virtual void addForeignWidget(Widget* w);
	virtual void addOwnedWidget(std::shared_ptr<Widget> w);
	virtual void removeForeignWidget(Widget* w);
	virtual void removeOwnedWidget(Widget* w);

	virtual void setLayout(int t);
	virtual void setOverflow(int p);
	virtual void setCenter(bool c);
	virtual void setStretch(bool s);
	virtual void setPadding(unsigned int h, unsigned int v);

protected:
	std::shared_ptr<Label> mLabel;
	std::shared_ptr<Widget> mContent;
};



class ButtonBox : public Box
{
public:
	ButtonBox(std::string title);
	virtual void setupChildren();

	virtual void addBottomButton(std::shared_ptr<Widget> w);
	virtual void addForeignBottomButton(Button* w);

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
	virtual void setText(std::string t);
	virtual const std::string& getText();

private:
	std::shared_ptr<Label> mLabel;
};



class Gui
{
public:
	void init();
	void paint();
	bool click(int x, int y);
	bool drag(int x, int y);
	bool drop(int x, int y);

	void setSceneSize(int w, int h);
	int getSceneWidth();
	int getSceneHeight();
	bool textInput(std::string t);

	bool grabsFocus();

	Widget& rootWidget();
	Console* getConsole();

	void message(std::string title, std::string msg);
	void showFps();

private:
	std::shared_ptr<Widget> mRoot;
	int mSceneWidth;
	int mSceneHeight;

	glm::mat4 mMvp;
	static Widget *mFocused;
	std::shared_ptr<Console> mConsole;
	std::shared_ptr<Label> mFps;

	friend Widget;
};

 
