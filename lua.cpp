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

	state["Label"].SetClass<Widget, const char *>("setPosition", &Widget::setPosition);
	state["Label"].SetClass<Label, std::string>("setText", &Label::setText);
	state["Box"].SetClass<Widget, const char *>("setRect", &Widget::setRect);
	state["Box"].SetClass<Widget, const char *>("addLabel", &Widget::addLabel);
	state["Box"].SetClass<Box>();
	state["Widget"].SetClass<Widget, const char *>("addLabel", &Widget::addLabel);
	state["Widget"].SetClass<Widget, const char *>("addBox", &Widget::addBox);

	state.Load("assets/lua.lua");
}

void Lua::run()
{
	state["setupGui"]();
}

