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
	state["gui"].SetObj<Gui>(*gui, "rootWidget", &Gui::rootWidget);

	state["Label"].SetClass<Widget, std::string>("setPosition", &Widget::setPosition);
	state["Label"].SetClass<Label, std::string>("setText", &Label::setText);
	// state["Box"].SetClass<Box>();
	state["Widget"].SetClass<Widget, std::string>("setRect", &Widget::setRect);
	state["Widget"].SetClass<Widget, std::string>("onClickLua", &Widget::onClickLua);
	state["Widget"].SetClass<Widget, std::string>("addLabel", &Widget::addLabel);
	state["Widget"].SetClass<Widget, std::string>("addWidget", &Widget::addWidget);

	state.Load("assets/lua.lua");
}

void Lua::run()
{
	state["setupGui"]();
}

