#include "gui.h"
#include "log.h"
#include "gui_sprite.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Widget::Widget(const char *texture)
{
	if(texture)
	{
		mSprite = std::make_shared<GuiSprite>();
		mSprite->init(texture, "gui");
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
}

void Widget::setSize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;
	updatePosition();
}

void Widget::setRect(unsigned int top, unsigned int left, unsigned int width, unsigned int height)
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

void Widget::addBox(Box* w)
{
	setupChild(w);
	mForeignWidgets.push_back(w);
}

void Widget::addLabel(Label* w)
{
	setupChild(w);
	mForeignWidgets.push_back(w);
}

void Widget::addWidget(std::shared_ptr<Widget> w)
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
	switch(mLayoutType)
	{
		case ltNone:
					w->updatePosition(); // Offsets changed and wont be called otherwse
					break;
		case ltHorizontal:
					for(auto w: mWidgets)
						tmp += w->mWidth;
					w->setLeft(tmp + mWidgets.size()*mSpacing + mPaddingHoris);
					w->setTop(mPaddingVert);
					break;
		case ltVertical:
					for(auto w: mWidgets)
						tmp += w->mHeight;
					w->setTop(tmp + mWidgets.size()*mSpacing + mPaddingVert);
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

		if(mOnClick) mOnClick();

		return true;
	}

	return false;
}

void Widget::onClick(std::function<void(void)> fnc)
{
	mOnClick = fnc;
}
//------------------------------------------------------------------------------
Label::Label(std::string text):
	Widget(nullptr)
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
	for(int i = 0; i < strlen(text); i++)
	{
		id[7] = text[i];
		w = std::make_shared<Widget>(id);
		w->setSize(15, 15);
		addWidget(w);
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
	Log() << "Gui init";
	mRoot = std::make_shared<Widget>(nullptr);

	mRoot->setLayout(Widget::ltNone);

	// auto b = std::make_shared<Box>(nullptr);
	// b->setRect(100,100, 320, 100);
	// mRoot->addWidget(b);
    //
	// auto w = std::make_shared<Label>(nullptr, "qwertyuiop{asdfghjklzxcvbnm<}");
	// // w->setPosition(50,50);
	// b->addWidget(w);
	// b->onClick([](){Log() << "box";});

	mUniformMVP = Shader::getShader("gui")->mkUniform("mvp");
}

void Gui::paint()
{
	glUniformMatrix4fv(mUniformMVP, 1, GL_FALSE, glm::value_ptr(mMvp));
	mRoot->paint();
}

void Gui::setSceneSize(int w, int h)
{
	mSceneWidth = w;
	mSceneHeight = h;

	mRoot->setWidth(w);
	mRoot->setHeight(h);

	mMvp = glm::ortho(0.f, 1.0f*mSceneWidth, 1.0f*mSceneHeight, 0.0f, -1.f, 1.f);
}

void Gui::click(int x, int y)
{
	mRoot->click(x, y);
}

Widget& Gui::rootWidget()
{
	return *mRoot.get();
}

