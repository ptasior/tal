#include "lua.h"
#include "log.h"
#include "gui.h"
#include <selene.h>

sel::State state{true};

void logFnc(std::string s)
{
	Log() << "-- " << s;
}

void Lua::init(Gui *gui)
{
	mGui = gui;

	state.HandleExceptionsWith([](int, std::string msg, std::exception_ptr){
			Log() << "Lua exception: " << msg;
		});

	state["log"] = &logFnc;
	state["gui"].SetObj<Gui>(*gui,
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight
		);

	applyWidgetInheritance("Label");
	state["Label"].SetClass<Label, std::string>("setText", &Label::setText);
	// state["Box"].SetClass<Box>();
	applyWidgetInheritance("Widget");

	state.Load("assets/lua.lua");
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
			"setColor", &Widget::setColor,

			"onClickLua", &Widget::onClickLua,

			"addLabel", &Widget::addLabel,
			"addWidget", &Widget::addWidget,
			"removeWidget", &Widget::removeWidget
		);
}

void Lua::run()
{
	state["setupGui"]();
}

