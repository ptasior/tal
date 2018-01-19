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

void Lua::initGui(Gui *gui)
{
	mGui = gui;

	state["gui"].SetObj<Gui>(*gui,
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight,
			"message", &Gui::message,
			"showFps", &Gui::showFps
		);

	applyWidgetInheritance("MultiLine");
	state["MultiLine"].SetClass<MultiLine, std::string>(
					"setText", &MultiLine::setText,
					"resize", &MultiLine::resize,
					"label", &MultiLine::label,
					"linesCount", &MultiLine::linesCount
				);

	applyWidgetInheritance("Label");
	state["Label"].SetClass<Label, std::string>(
					"setText", &Label::setText,
					"getText", &Label::getText
				);

	applyWidgetInheritance("Edit");
	state["Edit"].SetClass<Edit, std::string>(
					"setText", &Edit::setText,
					"getText", &Edit::getText
				);

	applyWidgetInheritance("Button");
	state["Button"].SetClass<Button, std::string>(
					"setText", &Button::setText
				);

	applyWidgetInheritance("Box");
	state["Box"].SetClass<Box, std::string>();

	applyWidgetInheritance("Scroll");
	state["Scroll"].SetClass<Scroll>();

	applyWidgetInheritance("Checkbox");
	state["Checkbox"].SetClass<Checkbox, bool>(
					"setChecked", &Checkbox::setChecked,
					"isChecked", &Checkbox::isChecked
				);

	applyWidgetInheritance("ButtonBox");
	state["ButtonBox"].SetClass<ButtonBox, std::string>(
			"addBottomButton", &ButtonBox::addForeignBottomButton
		);

	applyWidgetInheritance("Widget");
}

void Lua::applyWidgetInheritance(const char *type)
{
	state[type].SetClass<Widget, std::string>(
			"setTop", &Widget::setTop,
			"setLeft", &Widget::setLeft,
			"setWidth", &Widget::setWidth,
			"setHeight", &Widget::setHeight,
			"setPosition", &Widget::setPosition,
			"setSize", &Widget::setSize,
			"setRect", &Widget::setRect,
			"setLayout", &Widget::setLayout,
			"setOverflow", &Widget::setOverflow,
			"setCenter", &Widget::setCenter,
			"setStretch", &Widget::setStretch,
			"setColor", &Widget::setColor,

			"onClickLua", &Widget::onClickLua,

			"getTop", &Widget::getTop,
			"getLeft", &Widget::getLeft,
			"getWidth", &Widget::getWidth,
			"getHeight", &Widget::getHeight,

			"addWidget", &Widget::addWidget<Widget>,
			"addLabel", &Widget::addWidget<Label>,
			"addMultiLine", &Widget::addWidget<MultiLine>,
			"addEdit", &Widget::addWidget<Edit>,
			"addButton", &Widget::addWidget<Button>,
			"addBox", &Widget::addWidget<Box>,
			"addButtonBox", &Widget::addWidget<ButtonBox>,
			"addScroll", &Widget::addWidget<Scroll>,
			"addCheckbox", &Widget::addWidget<Checkbox>,

			"removeWidget", &Widget::removeWidget<Widget>,
			"removeLabel", &Widget::removeWidget<Label>,
			"removeMultiLine", &Widget::removeWidget<MultiLine>,
			"removeEdit", &Widget::removeWidget<Edit>,
			"removeButton", &Widget::removeWidget<Button>,
			"removeBox", &Widget::removeWidget<Box>,
			"removeButtonBox", &Widget::removeWidget<ButtonBox>,
			"removeScroll", &Widget::removeWidget<Scroll>
		);
}

