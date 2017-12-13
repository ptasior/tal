#include "gui.h"
#include "log.h"
#include "gui_sprite.h"
#include "shader.h"
#include "lua.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

Widget *Gui::mFocused = nullptr;

Widget::Widget(std::string texture)
{
	mSprite = std::make_shared<GuiSprite>();
	mSprite->init(texture.c_str(), "gui");
}

void Widget::updatePosition()
{
	// Update children
	for(auto w: mWidgets)
	{
		w->mLeftOffset = mLeftOffset + mLeft;
		w->mTopOffset = mTopOffset + mTop;
		w->updatePosition();
	}

	for(auto w: mForeignWidgets)
	{
		w->mLeftOffset = mLeftOffset + mLeft;
		w->mTopOffset = mTopOffset + mTop;
		w->updatePosition();
	}

	// Move own sprite
	if(!mSprite) return;

	glm::vec3 posv = glm::vec3(mLeft+mLeftOffset+0.5*mWidth, mTop+mTopOffset+0.5*mHeight, 0.0);
	glm::mat4 pos = glm::translate(glm::mat4(1.0f), posv);
	pos = glm::scale(pos, glm::vec3(0.5*mWidth, 0.5*mHeight, 0));
	mSprite->setPosition(pos);
}

void Widget::setTop(unsigned int v)
{
	mTop = v;
	updatePosition();
}

void Widget::setLeft(unsigned int v)
{
	mLeft = v;
	updatePosition();
}

void Widget::setWidth(unsigned int v)
{
	mWidth = v;
	setupChildren();
	updatePosition();
}

void Widget::setHeight(unsigned int v)
{
	mHeight = v;
	setupChildren();
	updatePosition();
}

void Widget::setPosition(unsigned int top, unsigned int left)
{
	mTop = top;
	mLeft = left;
	updatePosition();
}

void Widget::setSize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;
	setupChildren();
	updatePosition();
}

void Widget::setRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height)
{
	mTop = top;
	mLeft = left;
	mWidth = width;
	mHeight = height;
	setupChildren();
	updatePosition();
}

void Widget::setPadding(unsigned int h, unsigned int v)
{
	mPaddingHoris = h;
	mPaddingVert = v;

	setupChildren();
}

void Widget::addOwnedWidget(std::shared_ptr<Widget> w)
{
	mWidgets.push_back(w);
	setupChildren();
}

void Widget::addForeignWidget(Widget* w)
{
	mForeignWidgets.push_back(w);
	setupChildren();
}

void Widget::removeForeignWidget(Widget* w)
{
	auto f = std::remove_if(
				mForeignWidgets.begin(),
				mForeignWidgets.end(),
				[&w](Widget *i){return i == w;}
			);
	if(f == mForeignWidgets.end())
		Log() << "Widget: Cannot find widget to remove";
	else
		mForeignWidgets.erase(f, mForeignWidgets.end());
}

void Widget::setupChildren()
{
	int cnt = 0;

	for(auto w : mForeignWidgets)
		setupChild(w, cnt++);

	for(auto w : mWidgets)
		setupChild(w.get(), cnt++);
}

void Widget::setupChild(Widget* w, int pos)
{
	w->mParent = this;
	w->mLeftOffset = mLeftOffset + mLeft;
	w->mTopOffset = mTopOffset + mTop;

	int allEl = mForeignWidgets.size()+mWidgets.size();
	assert(pos < allEl);

	int tmp = 0;
	int cnt = 0;
	switch(mLayoutType)
	{
		case ltNone:
					w->updatePosition(); // Offsets changed and wont be called otherwse
					break;
		case ltHorizontal:
					for(auto w: mWidgets)
						if(cnt++ < pos)
							tmp += w->mWidth;
					for(auto w: mForeignWidgets)
						if(cnt++ < pos)
							tmp += w->mWidth;
					w->setLeft(tmp + pos*mSpacing + mPaddingHoris);
					w->setTop(mPaddingVert);
					break;
		case ltVertical:
					for(auto w: mWidgets)
						if(cnt++ < pos)
							tmp += w->mHeight;
					for(auto w: mForeignWidgets)
						if(cnt++ < pos)
							tmp += w->mHeight;
					w->setTop(tmp + pos*mSpacing + mPaddingVert);
					w->setLeft(mPaddingHoris);
					break;
	}
	switch(mOverflow)
	{
		case opNone: break;
		case opResize:
					if(mLayoutType == ltVertical)
					{
						w->setWidth(mWidth-2*mPaddingHoris);
						w->setHeight((mHeight-2*mPaddingVert -(allEl-1)*mSpacing)/(allEl));
					}
					if(mLayoutType == ltHorizontal)
					{
						w->setHeight(mHeight-2*mPaddingVert);
						w->setWidth((mWidth-2*mPaddingHoris -(allEl-1)*mSpacing)/(allEl));
					}
					break;
		case opClip:
					w->setVisible(
							w->mLeft+w->mWidth <= mWidth-2*mPaddingVert
							&&
							w->mTop+w->mHeight <= mHeight-2*mPaddingHoris
						);
					break;
	}

	if(mCenter)
		switch(mLayoutType)
		{
			case ltHorizontal:
						w->setTop(std::max(mPaddingVert, (mHeight-w->mHeight)/2));
						break;
			case ltVertical:
						w->setLeft(std::max(mPaddingHoris, (mWidth-w->mWidth)/2));
						break;
			default:
						Log(Log::DIE) << "Widget: ltNone layout and mCenter make no sense together";
						break;
		}
}

std::tuple<int, int, int, int> Widget::getRect()
{
	int l = mLeftOffset + mLeft;
	int t = mTopOffset + mTop;

	return std::make_tuple(l, t, l+mWidth, t+mHeight);
}

int Widget::getLeft()
{
	return mLeftOffset + mLeft;
}

int Widget::getTop()
{
	return mTopOffset + mTop;
}

void Widget::paint()
{
	if(!mVisible)
		return;

	if(mSprite)
		mSprite->paint();

	for(auto w :mWidgets)
		w->paint();

	for(auto f :mForeignWidgets)
		f->paint();
}

bool Widget::isPositionOver(int x, int y)
{
	int l, t, w, h;
	std::tie(l, t, w, h) = getRect();

	return l < x && x < w && t < y && y < h;
}

bool Widget::click(int x, int y)
{
	if(!isPositionOver(x, y)) return false;

	focus();

	for(auto w :mWidgets)
		if(w->click(x, y))
			return true;

	for(auto w :mForeignWidgets)
		if(w->click(x, y))
			return true;

	if(mOnClick)
	{
		mOnClick();
		return true;
	}
	if(mOnClickLua.size())
	{
		mOnClickLua[0]();
		return true;
	}

	return false;
}

bool Widget::drag(int x, int y)
{
	if(!isPositionOver(x, y)) return false;

	for(auto w :mWidgets)
		if(w->drag(x, y))
			return true;

	for(auto w :mForeignWidgets)
		if(w->drag(x, y))
			return true;

	return onDrag(x,y);
}

bool Widget::onDrag(int x, int y)
{
	return false;
}

bool Widget::drop(int x, int y)
{
	for(auto w :mWidgets)
		if(w->drop(x, y))
			return true;

	for(auto w :mForeignWidgets)
		if(w->drop(x, y))
			return true;

	return onDrop(x,y);
}

bool Widget::onDrop(int x, int y)
{
	return false;
}

void Widget::doFocus()
{
	Gui::mFocused = this;
}

void Widget::doUnfocus()
{
	Gui::mFocused = nullptr;
}

void Widget::focus()
{
}

void Widget::unfocus()
{
}

void Widget::onClick(std::function<void(void)> fnc)
{
	mOnClick = fnc;
}

void Widget::onClickLua(sel::function<void(void)> fnc)
{
	mOnClickLua.push_back(fnc);
}

void Widget::setColor(int r, int g, int b, int a)
{
	mSprite->setColor(r, g, b, a);
}

void Widget::setLayout(int t)
{
	mLayoutType = (LayoutType)t;
}

void Widget::setOverflow(int p)
{
	mOverflow = (OverflowPolicy)p;
}

void Widget::setCenter(bool c)
{
	mCenter = c;
}

void Widget::setVisible(bool v)
{
	mVisible = v;
}

bool Widget::isVisible()
{
	return mVisible;
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

	setText(text.c_str());
}

void Label::setText(const char *text)
{
	if(mWidgets.size())
		mWidgets.clear();

	std::shared_ptr<Widget> w;
	char id[] = "letter- ";
	unsigned int len = strlen(text);
	for(unsigned int i = 0; i < len; i++)
	{
		id[7] = text[i];
		w = std::make_shared<Widget>(id);
		w->setSize(10, 15);
		addOwnedWidget(w);
	}
	if(!mWidth && !mHeight)
		setSize(10*len+mSpacing*len+2*mPaddingHoris, 15+2*mPaddingVert);
}

//------------------------------------------------------------------------------
Edit::Edit(std::string text):
	Label(text)
{
	mText = text;
	setColor(mColorUnfocused,mColorUnfocused,mColorUnfocused,200);
}


std::string Edit::getText()
{
	return mText;
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
	if(t == "backspace")
	{
		if(mText.empty()) return;

		mText = mText.substr(0, mText.length()-1);
		Label::setText(mText.c_str());
	}
	else if(t == "return")
	{
		if(mOnReturn)
			mOnReturn();
	}
	else
	{
		mText = mText + t;
		Label::setText(mText.c_str());
	}
}

void Edit::setText(std::string t)
{
	mText = t;
	Label::setText(mText.c_str());
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

		return true;
	}

	bool onDrop(int x, int y)
	{
		mLastX = 0;
		mLastY = 0;
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
	mLabel->setText(title.c_str());

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
	auto l = std::make_shared<Label>(label);
	// l->setColor(100,0,0,200);
	addOwnedWidget(l);
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
		mLabels[i]->setText(mLines[i].c_str());
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
}

void Gui::paint()
{
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

void Gui::click(int x, int y)
{
	// TODO Mutex?
	if(mFocused)
		mFocused->unfocus();
	mRoot->click(x, y);
}

void Gui::drag(int x, int y)
{
	mRoot->drag(x, y);
}

void Gui::drop(int x, int y)
{
	mRoot->drop(x, y);
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

