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
		Log() << "Widget: Cannot find foreign widget to remove";
	else
		mForeignWidgets.erase(f, mForeignWidgets.end());

	Gui::mFocused = nullptr;
}

void Widget::removeOwnedWidget(Widget* w)
{
	auto f = std::remove_if(
				mWidgets.begin(),
				mWidgets.end(),
				[&w](std::shared_ptr<Widget> &i){return i.get() == w;}
			);
	if(f == mWidgets.end())
		Log() << "Widget: Cannot find widget to remove";
	else
		mWidgets.erase(f, mWidgets.end());

	Gui::mFocused = nullptr;
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
						if(mStretch)
							w->setHeight((mHeight-2*mPaddingVert -(allEl-1)*mSpacing)/(allEl));
					}
					if(mLayoutType == ltHorizontal)
					{
						w->setHeight(mHeight-2*mPaddingVert);
						if(mStretch)
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

void Widget::setStretch(bool s)
{
	mStretch = s;
}

void Widget::setVisible(bool v)
{
	mVisible = v;
}

bool Widget::isVisible()
{
	return mVisible;
}

