#include "gui.h"
#include "log.h"
#include "sprite_gui.h"
#include "shader.h"
#include "lua.h"
#include "time.h"
#include "config.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

const unsigned int Label::WIDTH = 9;
const unsigned int Label::HEIGHT = 15;

Widget *Gui::mFocused = nullptr;

Checkbox::Checkbox(bool s):
	Widget("")
{
	mCheckedSprite = std::make_shared<GuiSprite>();
	mCheckedSprite->init("letter-\13", "gui");
	mUnCheckedSprite = std::make_shared<GuiSprite>();
	mUnCheckedSprite->init("letter-\12", "gui");

	mChecked = s;
	mOnClick = [this](){mChecked = !mChecked; update();};

	update();
}

void Checkbox::update()
{
	if(mChecked) mSprite = mCheckedSprite;
	else mSprite = mUnCheckedSprite;
	updatePosition();
}

void Checkbox::setChecked(bool s)
{
	mChecked = s;
	update();
}

bool Checkbox::isChecked()
{
	return mChecked;
}

//------------------------------------------------------------------------------
class ScrollContent : public Widget
{
public:
	ScrollContent():
		Widget("")
	{}

	void setupChildren()
	{
		int cnt = mSkipWidgets;

		for(; cnt < mForeignWidgets.size(); cnt++)
			setupChild(mForeignWidgets[cnt], cnt-mSkipWidgets);

		for(; cnt < mWidgets.size(); cnt++)
			setupChild(mWidgets[cnt].get(), cnt-mSkipWidgets);
	}

	void setupChild(Widget* w, int pos)
	{
		Widget::setupChild(w,pos);

		int cnt = 0;
		for(auto w : mForeignWidgets)
		{
			if(cnt++ < mSkipWidgets)
				w->setVisible(false);
			else if(w->getTop() + w->getHeight() > getTop() + mHeight - mPaddingVert*2)
				w->setVisible(false);
			else
				w->setVisible(true);
		}

		for(auto w : mWidgets)
		{
			if(cnt++ < mSkipWidgets)
				w->setVisible(false);
			else if(w->getTop() + w->getHeight() > getTop() + mHeight - mPaddingVert*2)
				w->setVisible(false);
			else
				w->setVisible(true);
		}
	}

	void up()
	{
		mSkipWidgets = std::max(mSkipWidgets-1, 0);
		setupChildren();
	}

	void down()
	{
		mSkipWidgets = std::min(mSkipWidgets-1, (int)(mWidgets.size() + mForeignWidgets.size())-1);
		setupChildren();
	}

	void clear()
	{
		mForeignWidgets.clear();
		mWidgets.clear();
	}

	int mSkipWidgets = 0;
};

Scroll::Scroll():
	 Widget("")
{
	mLayoutType = ltNone;
	mPaddingHoris = 15;
	mPaddingVert = 15;
	mStretch = false;
	mSprite->setColor(150,150,150,200);

	mButtonUp = std::make_shared<Button>(std::string("\1"));
	mButtonUp->onClick([this](){onUp();});
	mButtonUp->setColor(50,50,50,200);
	mButtonUp->setPadding(3, 2);

	mButtonDown = std::make_shared<Button>(std::string("\2"));
	mButtonDown->onClick([this](){onDown();});
	mButtonDown->setColor(50,50,50,200);
	mButtonDown->setPadding(3, 2);


	mContent = std::make_shared<ScrollContent>();
	mContent->setColor(0,0,0,0);
	mContent->setLayout(ltVertical);
	mContent->setOverflow(opResize);
	mContent->setStretch(false);

	Widget::addOwnedWidget(mButtonUp);
	Widget::addOwnedWidget(mContent);
	Widget::addOwnedWidget(mButtonDown);

	setupChildren();
}

void Scroll::setupChildren()
{
	if(mButtonUp)
		mButtonUp->setRect(0, 0, mWidth, 23);
	if(mButtonDown)
		mButtonDown->setRect(0, mHeight-20, mWidth, 23);
	if(mContent)
		mContent->setRect(0, 20, mWidth, mHeight-40);

	Widget::setupChildren();
}

void Scroll::addForeignWidget(Widget* w)
{
	mContent->addForeignWidget(w);
}

void Scroll::addOwnedWidget(std::shared_ptr<Widget> w)
{
	mContent->addOwnedWidget(w);
}

void Scroll::removeForeignWidget(Widget* w)
{
	mContent->removeForeignWidget(w);
}

void Scroll::removeOwnedWidget(Widget* w)
{
	mContent->removeOwnedWidget(w);
}

void Scroll::onUp()
{
	static_cast<ScrollContent*>(mContent.get())->up();
}

void Scroll::onDown()
{
	static_cast<ScrollContent*>(mContent.get())->down();
}

void Scroll::clear()
{
	static_cast<ScrollContent*>(mContent.get())->clear();
}



//------------------------------------------------------------------------------
MultiLine::MultiLine(std::string text)
{
	mLayoutType = ltVertical;
	mOverflow = opClip;
	mStretch = false;
	mSpacing = 0;
	mPaddingVert = 0;
	mPaddingHoris = 0;

	setColor(0,0,0,0);

	setText(text);
}

void MultiLine::setText(std::string text)
{
	mLines.clear();
	mLabels.clear();

	if(text.empty()) return;

	std::string tmp;
	for(unsigned int i = 0; i < text.length(); i++)
		if(text[i] != '\n')
			tmp += text[i];
		else
		{
			mLines.push_back(tmp);
			tmp.clear();
		}
	mLines.push_back(tmp);

	for(auto l : mLines)
	{
		auto lbl = std::make_shared<Label>(l);
		mLabels.push_back(lbl);
		addOwnedWidget(lbl);
	}
}

void MultiLine::resize()
{
	int maxlen = 0;
	for(auto l : mLines)
		maxlen = std::max(maxlen, (int)l.length());

	setSize(maxlen*Label::WIDTH, mLabels.size()*Label::HEIGHT);
}

Label* MultiLine::label(int l)
{
	return mLabels[l].get();
}

int MultiLine::linesCount()
{
	return mLabels.size();
}

//------------------------------------------------------------------------------
Label::Label(std::string text):
	Widget("")
{
	mLayoutType = ltHorizontal;
	mOverflow = opClip;
	mSpacing = 0;
	mPaddingVert = 0;
	mPaddingHoris = 0;
	mCenter = true;
	setColor(0,0,0,0);

	setTextColor(255,255,255, 255);
	setText(text);
}

void Label::setText(std::string text)
{
	mText = text;
	mWidgets.clear();

	std::shared_ptr<Widget> w;
	char id[] = "letter- ";
	unsigned int len = text.size();

	for(unsigned int i = 0; i < len; i++)
	{
		id[7] = text[i];
		w = std::make_shared<Widget>(id);
		w->setSize(Label::WIDTH, Label::HEIGHT);
		w->setColor(mColor.x, mColor.y, mColor.z, mColor.w);
		addOwnedWidget(w);
	}
	if(!mWidth && !mHeight)
		setSize(Label::WIDTH*len+mSpacing*len+2*mPaddingHoris, Label::HEIGHT+2*mPaddingVert);
}

const std::string& Label::getText()
{
	return mText;
}

void Label::setTextColor(int r, int g, int b, int a)
{
	mColor.x = r;
	mColor.y = g;
	mColor.z = b;
	mColor.w = a;
}

//------------------------------------------------------------------------------
Edit::Edit(std::string text):
	Label(text)
{
	setColor(mColorUnfocused,mColorUnfocused,mColorUnfocused,200);
}

void Edit::focus()
{
	Widget::doFocus();
	setColor(mColorFocused,mColorFocused,mColorFocused,200);
}

void Edit::unfocus()
{
	Widget::doUnfocus();
	setColor(mColorUnfocused,mColorUnfocused,mColorUnfocused,200);
}

void Edit::addText(std::string t)
{
	std::string text = getText();
	if(t == "backspace")
	{
		if(text.empty()) return;

		Label::setText(text.substr(0, text.length()-1));
	}
	else if(t == "return")
	{
		if(mOnReturn)
			mOnReturn();
	}
	else
		Label::setText(text + t);
}

const std::string& Edit::getText()
{
	return Label::getText();
}

void Edit::setText(std::string t)
{
	Label::setText(t);
}

void Edit::setOnReturn(std::function<void(void)> fnc)
{
	mOnReturn = fnc;
}

//------------------------------------------------------------------------------
class BoxLabelDrag : public Label
{
public:
	BoxLabelDrag(std::string text, Widget*box):
		Label(text),
		mBox(box)
	{}

	bool onDrag(int x, int y)
	{
		if(mLastX == 0) mLastX = x;
		if(mLastY == 0) mLastY = y;

		mBox->setLeft(mBox->getLeft() + x-mLastX);
		mBox->setTop(mBox->getTop() + y-mLastY);

		mLastX = x;
		mLastY = y;
		Widget::doFocus();

		return true;
	}

	bool onDrop(int x, int y)
	{
		mLastX = 0;
		mLastY = 0;
		Widget::doUnfocus();

		return true;
	}


private:
	Widget *mBox;
	int mLastX = 0;
	int mLastY = 0;
};

Box::Box(std::string title):
	 Widget("")
{
	mLayoutType = ltNone;
	mPaddingHoris = 15;
	mPaddingVert = 15;
	mSprite->setColor(150,150,150,200);

	mLabel = std::make_shared<BoxLabelDrag>(title, this);
	mLabel->setColor(50,50,50,200);
	mLabel->setPadding(3, 2);
	// mLabel->setText(title);

	Widget::addOwnedWidget(mLabel);

	mContent = std::make_shared<Widget>();
	mContent->setColor(0,0,0,0);
	mContent->setLayout(ltVertical);
	mContent->setCenter(true);
	mContent->setOverflow(opResize);
	Widget::addOwnedWidget(mContent);
}

void Box::setupChildren()
{
	if(mLabel)
		mLabel->setRect(0, 0, mWidth, 23);
	if(mContent)
		mContent->setRect(0, 20, mWidth, mHeight-23);
	Widget::setupChildren();
}

void Box::setTitle(std::string title)
{
	mLabel->setText(title);
}

void Box::addForeignWidget(Widget* w)
{
	mContent->addForeignWidget(w);
}

void Box::addOwnedWidget(std::shared_ptr<Widget> w)
{
	mContent->addOwnedWidget(w);
}

void Box::removeForeignWidget(Widget* w)
{
	mContent->removeForeignWidget(w);
}

void Box::removeOwnedWidget(Widget* w)
{
	mContent->removeOwnedWidget(w);
}

void Box::setLayout(int t)
{
	mContent->setLayout(t);
}

void Box::setOverflow(int p)
{
	mContent->setOverflow(p);
}

void Box::setCenter(bool c)
{
	mContent->setCenter(c);
}

void Box::setStretch(bool s)
{
	mContent->setStretch(s);
}

void Box::setPadding(unsigned int h, unsigned int v)
{
	mContent->setPadding(h, v);
}

//------------------------------------------------------------------------------
ButtonBox::ButtonBox(std::string title):
	 Box(title)
{
	mButtonWidget = std::make_shared<Widget>();
	mButtonWidget->setLayout(ltHorizontal);
	mButtonWidget->setOverflow(opResize);
	mButtonWidget->setCenter(true);
	mButtonWidget->setColor(0,0,0,0);

	Widget::addOwnedWidget(mButtonWidget);
}

void ButtonBox::setupChildren()
{
	if(mLabel)
		mLabel->setRect(0, 0, mWidth, 23);
	if(mContent)
		mContent->setRect(0, 20, mWidth, mHeight-60);
	if(mButtonWidget)
		mButtonWidget->setRect(0, mHeight-40, mWidth, 40);
	Widget::setupChildren();
}

void ButtonBox::addBottomButton(std::shared_ptr<Widget> w)
{
	mButtonWidget->addOwnedWidget(w);
}

void ButtonBox::addForeignBottomButton(Button* w)
{
	mButtonWidget->addForeignWidget(w);
}

//------------------------------------------------------------------------------
Button::Button(std::string label):
	 Widget("")
{
	mLayoutType = ltVertical;
	mPaddingVert = 10;
	mCenter = true;
	mOverflow = opNone;

	mSprite->setColor(100,100,100,200);
	mLabel = std::make_shared<Label>(label);
	// l->setColor(100,0,0,200);
	addOwnedWidget(mLabel);
}

const std::string& Button::getText()
{
	return mLabel->getText();
}

void Button::setText(std::string t)
{
	mLabel->setText(t);
}

//------------------------------------------------------------------------------
Console::Console():
	Widget("")
{
	mSpacing = 0;
	setLayout(ltVertical);
	setOverflow(opResize);
	setVisible(false);

	setRect(0,0, 400, 400);
	setColor(0,0,0,200);

	for(unsigned int i = 0; i < LINESCNT; i++)
	{
		auto l = std::make_shared<Label>("");
		addOwnedWidget(l);
		mLabels.push_back(l);
	}

	mEdit = std::make_shared<Edit>("");
	mEdit->mColorUnfocused = 0;
	mEdit->mColorFocused = 0;
	mEdit->setColor(0,0,0,200);
	mEdit->setOnReturn([this](){
			std::string cmd = mEdit->getText();
			mEdit->setText("");
			log(cmd);

			Lua::getInstance()->execute(cmd.c_str());
		});
	addOwnedWidget(mEdit);
}

void Console::log(std::string &log)
{
	mLines.push_back(log);

	while(mLines.size() > LINESCNT)
		mLines.erase(mLines.begin());

	for(unsigned int i = 0; i < mLines.size(); i++)
		mLabels[i]->setText(mLines[i]);
}

//------------------------------------------------------------------------------
void Gui::init()
{
	Log() << "Gui: init";
	mRoot = std::make_shared<Widget>();

	mRoot->setLayout(Widget::ltNone);
	mRoot->setOverflow(Widget::opNone);
	mRoot->mSprite.reset(); // Do not display anything

	mConsole = std::make_shared<Console>();
	mRoot->addOwnedWidget(mConsole);
	if(global_config->get("showFps") == "true")
		showFps();
}

void Gui::paint()
{
	if(mFps) mFps->setText("FPS: "+std::to_string((int)Time::fps()));
	Shader::getShader("gui")->use();

	mRoot->paint();
}

void Gui::setSceneSize(int w, int h)
{
	mSceneWidth = w;
	mSceneHeight = h;

	mRoot->setWidth(w);
	mRoot->setHeight(h);
	mConsole->setWidth(w);

	mMvp = glm::ortho(0.f, 1.0f*mSceneWidth, 1.0f*mSceneHeight, 0.0f, -1.f, 1.f);
	Shader::getShader("gui")->setUniform("mvp", {glm::value_ptr(mMvp)});
}

bool Gui::click(int x, int y)
{
	// TODO Mutex?
	if(mFocused)
		mFocused->unfocus();
	return mRoot->click(x, y);
}

bool Gui::drag(int x, int y)
{
	return mRoot->drag(x, y);
}

bool Gui::drop(int x, int y)
{
	return mRoot->drop(x, y);
}

Widget& Gui::rootWidget()
{
	return *mRoot.get();
}

int Gui::getSceneWidth()
{
	return mSceneWidth;
}

int Gui::getSceneHeight()
{
	return mSceneHeight;
}

bool Gui::textInput(std::string t)
{
	if(dynamic_cast<Edit*>(mFocused))
	{
		dynamic_cast<Edit*>(mFocused)->addText(t);
		return true;
	}

	if(t == "~")
	{
		mConsole->setVisible(!mConsole->isVisible());
		return true;
	}

	return false;
}

Console* Gui::getConsole()
{
	return mConsole.get();
}

bool Gui::grabsFocus()
{
	return mFocused;
}

void Gui::message(std::string title, std::string msg)
{
	auto box = std::make_shared<ButtonBox>(title);
	int width = 10+msg.length()*10;
	box->setRect((mSceneWidth-width)/2, (mSceneHeight-100)/2, width, 100);

	auto btn = std::make_shared<Button>("OK");
	btn->onClick([box, this](){
			Lua::setWait(0);
			mRoot->removeOwnedWidget(box.get());
		});
	box->addBottomButton(btn);

	auto lbl = std::make_shared<Label>(msg);
	box->addOwnedWidget(lbl);

	mRoot->addOwnedWidget(box);
}

void Gui::showFps()
{
	if(!mFps)
	{
		mFps = std::make_shared<Label>("");
		mFps->setTextColor(255, 0,0, 200);
		mFps->setRect(0,0, Label::WIDTH*10, Label::HEIGHT);
		mRoot->addOwnedWidget(mFps);
	}
	else
	{
		mRoot->removeOwnedWidget(mFps.get());
		mFps.reset();
	}
}

