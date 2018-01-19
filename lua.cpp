#include "lua.h"
#include "log.h"
#include "gui.h"
#include "scene.h"
#include "sprite.h"
#include "model_obj.h"
#include "skybox.h"
#include "map.h"
#include "matrix.h"
#include "time.h"
#include "config.h"
#include "shared_data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Lua::logFnc(std::string s)
{
	Log() << "-- " << s;
	Gui* gui = getInstance()->mGui;
	if(gui) gui->getConsole()->log(s);
}

Lua* Lua::getInstance()
{
	static Lua l;
	return &l;
}

Lua::Lua():
	state(true)
{
	state["log"] = &Lua::logFnc;
	state["wireframe"] = &Lua::wireframe;
	state["setLoopResolution"] = &Lua::setLoopResolution;
	state["setWait"] = &Lua::setWait;
	state["setTimeout"] = &Lua::setTimeout;

	state["sharedData"].SetObj<SharedData>(SharedData::root(),
			"at", &SharedData::at,
			"print", &SharedData::print
		);

	state["SharedData"].SetClass<SharedData>(
			"at", &SharedData::at,
			"get", &SharedData::get,
			"set", &SharedData::set
		);

}

void Lua::setup()
{
	state.HandleExceptionsWith([this](int, std::string msg, std::exception_ptr){
			if(mGui) mGui->getConsole()->log(msg);
			Log() << "----------------------------------------";
			Log() << "Lua exception: " << msg;
			Log() << "----------------------------------------";
		});

	state.Load(global_config->get("gameFile").c_str());
	state.Load("lua_lib/main.lua");

	state["setup"]();
}

void Lua::loop()
{
	static unsigned int time = Time::current();

	if(time + mLoopResolution > Time::current())
		return; // Don't call every frame

	if(mWait == wsRefresh) // Run one frame
		mWait = wsRun;

	if(mWait == wsSleep && Time::current() > mTimeout)
		mWait = wsRun;

	time = Time::current();

	if(mWait == wsRun)
		state["main_loop"]();
}

void Lua::execute(const char *cmd)
{
	state(cmd);
}

void Lua::wireframe()
{
	static bool wf = false;
	wf = !wf;
#ifndef __EMSCRIPTEN__
	glPolygonMode(GL_FRONT_AND_BACK, wf?GL_LINE:GL_FILL);
#endif
}

void Lua::setWait(int v)
{
	getInstance()->mWait = (WaitState)v;
}

void Lua::setTimeout(int v)
{
	getInstance()->mTimeout = Time::current() + v;
}

void Lua::setLoopResolution(unsigned int res)
{
	getInstance()->mLoopResolution = res;
}

void Lua::resizeWindow()
{
	state["resizeWindow"]();
}

void Lua::sharedDataUpdated(const std::string &line)
{
	state["sharedDataUpdated"](line);
}

