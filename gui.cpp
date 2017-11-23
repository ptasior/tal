#include "gui.h"
#include "log.h"
#include "sprite.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Widget::init(Widget* parent, const char *texture)
{
	mParent = parent;
	if(texture)
	{
		mSprite = std::make_shared<Sprite>();
		mSprite->init(texture, "gui");
	}
}

void Widget::updatePosition()
{
	if(!mSprite) return;

	glm::mat4 mPositionMatrix =
		glm::translate(glm::mat4(1.0f),
				glm::vec3(mLeft+mLeftOffset+0.5*mHeight, mTop+mTopOffset+0.5*mWidth, 0.0))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.5*mHeight, 0.5*mWidth, 0));
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
}

void Widget::setLayout(LayoutType t)
{
	mLayoutType = t;
}

void Widget::addWidget(std::shared_ptr<Widget> w)
{
	int tmp = 0;
	w->mParent = this;
	w->mLeftOffset = mLeftOffset + mLeft;
	w->mTopOffset = mTopOffset + mTop;
	Log() <<  "topOffest " <<w->mTopOffset; 
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
	mWidgets.push_back(w);
}

//------------------------------------------------------------------------------
Label::Label(Widget * parent, const char *text)
{
	mLayoutType = ltHorizontal;
	mParent = parent;
	mSpacing = -5;

	setText(text);
}

void Label::setText(const char *text)
{
	if(mWidgets.size())
		mWidgets.clear(); // TODO Make sure shared_ptrs work

	std::shared_ptr<Widget> w;
	char id[] = "letter- ";
	for(int i = 0; i < strlen(text); i++)
	{
		w = std::make_shared<Widget>();
		id[7] = text[i];
		w->init(nullptr, id);
		w->setSize(15, 15);
		addWidget(w);
	}
}
//------------------------------------------------------------------------------

void Gui::init()
{
	Log() << "Gui init";
	mRoot = std::make_shared<Widget>();
	mRoot->init(nullptr, nullptr);

	mRoot->setLayout(Widget::ltNone);

	// auto w = std::make_shared<Label>(nullptr, "!@#$%^&*(){}:\"<>?./;'[]\\|'");
	auto w = std::make_shared<Label>(nullptr, "qwertyuiop{asdfghjklzxcvbnm<}");
	w->setPosition(100,100);
	mRoot->addWidget(w);


	// auto w = std::make_shared<Widget>();
	// w->init(mRoot.get(), "letter-a");
	// w->setRect(10, 10, 15, 15);
	// mRoot->addWidget(w);
    //
	// w = std::make_shared<Widget>();
	// w->init(mRoot.get(), "letter-b");
	// w->setRect(10, 25, 15, 15);
	// mRoot->addWidget(w);
    //
	// w = std::make_shared<Widget>();
	// w->init(mRoot.get(), "letter-c");
	// w->setRect(10, 40, 15, 15);
	// mRoot->addWidget(w);

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

