#include "gui.h"
#include "log.h"
#include "gui_sprite.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

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

bool Widget::click(int x, int y)
{
	int l, t, w, h;
	std::tie(l, t, w, h) = getRect();

	if(l < x && x < w && t < y && y < h)
	{
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
	}

	return false;
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

void Widget::setVisible(bool v)
{
	mVisible = v;
}

void Widget::setCenter(bool c)
{
	mCenter = c;
}

//------------------------------------------------------------------------------
Label::Label(std::string &text):
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
Box::Box(std::string &title):
	 Widget("")
{
	mLayoutType = ltNone;
	mPaddingHoris = 15;
	mPaddingVert = 15;
	mSprite->setColor(150,150,150,200);

	mLabel = std::make_shared<Label>(title);
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
Button::Button(std::string &label):
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

void Gui::init()
{
	Log() << "Gui: init";
	mRoot = std::make_shared<Widget>();

	mRoot->setLayout(Widget::ltNone);

	// auto b = std::make_shared<Box>(nullptr);
	// b->setRect(100,100, 320, 100);
	// mRoot->addWidget(b);
    //
	// auto w = std::make_shared<Label>(nullptr, "qwertyuiop{asdfghjklzxcvbnm<}");
	// // w->setPosition(50,50);
	// b->addWidget(w);
	// b->onClick([](){Log() << "box";});
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
	mRoot->setOverflow(Widget::opResize);
	mRoot->mSprite.reset();

	mMvp = glm::ortho(0.f, 1.0f*mSceneWidth, 1.0f*mSceneHeight, 0.0f, -1.f, 1.f);
	Shader::getShader("gui")->setUniform("mvp", {glm::value_ptr(mMvp)});
}

void Gui::click(int x, int y)
{
	mRoot->click(x, y);
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

