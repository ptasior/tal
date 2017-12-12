#include "gui.h"
#include "log.h"
#include "gui_sprite.h"
#include "shader.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Widget::Widget(std::string texture)
{
	if(!texture.empty())
	{
		mSprite = std::make_shared<GuiSprite>();
		mSprite->init(texture.c_str(), "gui");
	}
}

void Widget::updatePosition()
{
	if(!mSprite) return;

	glm::mat4 mPositionMatrix =
		glm::translate(glm::mat4(1.0f),
				glm::vec3(mLeft+mLeftOffset+0.5*mWidth, mTop+mTopOffset+0.5*mHeight, 0.0))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.5*mWidth, 0.5*mHeight, 0));
	mSprite->setPosition(mPositionMatrix);
}

void Widget::setPosition(unsigned int top, unsigned int left)
{
	mTop = top;
	mLeft = left;
	updatePosition();

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
}

void Widget::setSize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;
	updatePosition();
}

void Widget::setRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height)
{
	mTop = top;
	mLeft = left;
	mWidth = width;
	mHeight = height;
	updatePosition();

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
}

void Widget::setTop(unsigned int v)
{
	mTop = v;
	updatePosition();

	for(auto w: mWidgets)
	{
		w->mTopOffset = mTopOffset + mTop;
		w->updatePosition();
	}

	for(auto w: mForeignWidgets)
	{
		w->mTopOffset = mTopOffset + mTop;
		w->updatePosition();
	}
}

void Widget::setLeft(unsigned int v)
{
	mLeft = v;
	updatePosition();

	for(auto w: mWidgets)
	{
		w->mLeftOffset = mLeftOffset + mLeft;
		w->updatePosition();
	}

	for(auto w: mForeignWidgets)
	{
		w->mLeftOffset = mLeftOffset + mLeft;
		w->updatePosition();
	}
}

void Widget::setWidth(unsigned int v)
{
	mWidth = v;
	updatePosition();
}

void Widget::setHeight(unsigned int v)
{
	mHeight = v;
	updatePosition();
}

void Widget::paint()
{
	if(mSprite)
		mSprite->paint();

	for(auto w :mWidgets)
		w->paint();

	for(auto f :mForeignWidgets)
		f->paint();
}

void Widget::setLayout(LayoutType t)
{
	mLayoutType = t;
}

void Widget::removeWidget(Widget* w)
{
	mForeignWidgets.erase(std::remove_if(
				mForeignWidgets.begin(),
				mForeignWidgets.end(),
				[&w](Widget *i){return i == w;}
			), mForeignWidgets.end());
}

void Widget::addOwnedWidget(std::shared_ptr<Widget> w)
{
	setupChild(w.get());
	mWidgets.push_back(w);
}

void Widget::setupChild(Widget* w)
{
	int tmp = 0;
	w->mParent = this;
	w->mLeftOffset = mLeftOffset + mLeft;
	w->mTopOffset = mTopOffset + mTop;
	int widgetsCnt = mForeignWidgets.size()+mWidgets.size();
	switch(mLayoutType)
	{
		case ltNone:
					w->updatePosition(); // Offsets changed and wont be called otherwse
					break;
		case ltHorizontal:
					for(auto w: mWidgets)
						tmp += w->mWidth;
					for(auto w: mForeignWidgets)
						tmp += w->mWidth;
					w->setLeft(tmp + widgetsCnt*mSpacing + mPaddingHoris);
					w->setTop(mPaddingVert);
					break;
		case ltVertical:
					for(auto w: mWidgets)
						tmp += w->mHeight;
					for(auto w: mForeignWidgets)
						tmp += w->mHeight;
					w->setTop(tmp + widgetsCnt*mSpacing + mPaddingVert);
					w->setLeft(mPaddingHoris);
					break;
	}
}

std::tuple<int, int, int, int> Widget::getRect()
{
	int l = mLeftOffset + mLeft;
	int t = mTopOffset + mTop;

	return std::make_tuple(l, t, l+mWidth, t+mHeight);
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
			mOnClick();
		if(mOnClickLua.size())
			mOnClickLua[0]();

		return true;
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

//------------------------------------------------------------------------------
Label::Label(std::string text):
	Widget("")
{
	mLayoutType = ltHorizontal;
	mSpacing = -5;

	setText(text.c_str());
}

void Label::setText(const char *text)
{
	if(mWidgets.size())
		mWidgets.clear();

	std::shared_ptr<Widget> w;
	char id[] = "letter- ";
	for(unsigned int i = 0; i < strlen(text); i++)
	{
		id[7] = text[i];
		w = std::make_shared<Widget>(id);
		w->setSize(15, 15);
		addOwnedWidget(w);
	}
}

//------------------------------------------------------------------------------
Box::Box():
	 Widget("assets/gui/box.png")
{
	mLayoutType = ltVertical;
	mSprite->setColor(255,0,255,255);
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

