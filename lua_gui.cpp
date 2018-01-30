#include "lua.h"
#include "log.h"
#include "gui.h"
#include "sprite.h"
#include "time.h"
#include "config.h"
#include "shared_data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define WIDGET_INHERITED \
			"setTop", &Widget::setTop, \
			"setLeft", &Widget::setLeft, \
			"setWidth", &Widget::setWidth, \
			"setHeight", &Widget::setHeight, \
			"setPosition", &Widget::setPosition, \
			"setSize", &Widget::setSize, \
			"setRect", &Widget::setRect, \
			"setPadding", &Widget::setPadding, \
			"setLayout", &Widget::setLayout, \
			"setOverflow", &Widget::setOverflow, \
			"setCenter", &Widget::setCenter, \
			"setStretch", &Widget::setStretch, \
			"setColor", &Widget::setColor, \
 \
			"onClickLua", &Widget::onClickLua, \
 \
			"getTop", &Widget::getTop, \
			"getLeft", &Widget::getLeft, \
			"getWidth", &Widget::getWidth, \
			"getHeight", &Widget::getHeight, \
 \
			"addWidget", &Widget::addWidget<Widget>, \
			"addLabel", &Widget::addWidget<Label>, \
			"addMultiLine", &Widget::addWidget<MultiLine>, \
			"addEdit", &Widget::addWidget<Edit>, \
			"addButton", &Widget::addWidget<Button>, \
			"addBox", &Widget::addWidget<Box>, \
			"addButtonBox", &Widget::addWidget<ButtonBox>, \
			"addScroll", &Widget::addWidget<Scroll>, \
			"addCheckbox", &Widget::addWidget<Checkbox>, \
 \
			"removeWidget", &Widget::removeWidget<Widget>, \
			"removeLabel", &Widget::removeWidget<Label>, \
			"removeMultiLine", &Widget::removeWidget<MultiLine>, \
			"removeEdit", &Widget::removeWidget<Edit>, \
			"removeButton", &Widget::removeWidget<Button>, \
			"removeBox", &Widget::removeWidget<Box>, \
			"removeButtonBox", &Widget::removeWidget<ButtonBox>, \
			"removeScroll", &Widget::removeWidget<Scroll>



void Lua::initGui(Gui *gui)
{
	mGui = gui;

	mState["gui"] = gui;
	mState.new_usertype<Gui>("Gui",
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight,
			"message", &Gui::message,
			"showFps", &Gui::showFps
		);

	mState.new_usertype<MultiLine>("MultiLine",
					sol::constructors<MultiLine(std::string)>(),
					"setText", &MultiLine::setText,
					"resize", &MultiLine::resize,
					"label", &MultiLine::label,
					"linesCount", &MultiLine::linesCount,
					WIDGET_INHERITED
				);

	mState.new_usertype<Label>("Label",
					sol::constructors<Label(std::string)>(),
					"setText", &Label::setText,
					"getText", &Label::getText,
					WIDGET_INHERITED
				);

	mState.new_usertype<Edit>("Edit",
					sol::constructors<Edit(std::string)>(),
					"setText", &Edit::setText,
					"getText", &Edit::getText,
					WIDGET_INHERITED
				);

	mState.new_usertype<Button>("Button",
					sol::constructors<Button(std::string)>(),
					"setText", &Button::setText,
					WIDGET_INHERITED
				);

	mState.new_usertype<Box>("Box",
					sol::constructors<Box(std::string)>(),
					WIDGET_INHERITED
				);

	mState.new_usertype<Scroll>("Scroll",
					"clear", &Scroll::clear,
					WIDGET_INHERITED
				);

	mState.new_usertype<Checkbox>("Checkbox",
					sol::constructors<Checkbox(bool)>(),
					"setChecked", &Checkbox::setChecked,
					"isChecked", &Checkbox::isChecked,
					WIDGET_INHERITED
				);

	mState.new_usertype<ButtonBox>("ButtonBox",
					sol::constructors<ButtonBox(std::string)>(),
					"addBottomButton", &ButtonBox::addForeignBottomButton,
					WIDGET_INHERITED
				);

	mState.new_usertype<Widget>("Widget",
					sol::constructors<Widget(std::string)>(),
					WIDGET_INHERITED
				);
}

