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

int luaPanic(lua_State* L)
{
	// if(mGui) mGui->getConsole()->log(msg);
	std::string msg;

	sol::optional<sol::string_view> maybetopmsg = sol::stack::check_get<sol::string_view>(L, 1);
	if (maybetopmsg) {
		const sol::string_view& topmsg = maybetopmsg.value();
		msg.assign(topmsg.data(), topmsg.size());
	}

	luaL_traceback(L, L, msg.c_str(), 1);
	sol::optional<sol::string_view> maybetraceback = sol::stack::check_get<sol::string_view>(L, -1);
	if (maybetraceback) {
		const sol::string_view& traceback = maybetraceback.value();
		msg.assign(traceback.data(), traceback.size());
	}

	Log(Log::DIE)<< "Lua exception: "
				<< sol::stack::get<std::string>(L, -1);
	return -1;
}

Lua::Lua():
	mState(sol::c_call<decltype(&luaPanic), &luaPanic>)
{
	mState.open_libraries(sol::lib::base,
						sol::lib::package,
						sol::lib::coroutine,
						sol::lib::string,
						sol::lib::os,
						sol::lib::math,
						sol::lib::table,
						sol::lib::debug
					);


	mState["log"] = &Lua::logFnc;
	mState["wireframe"] = &Lua::wireframe;
	mState["setLoopResolution"] = &Lua::setLoopResolution;
	mState["setWait"] = &Lua::setWait;
	mState["setTimeout"] = &Lua::setTimeout;

	mState.new_usertype<SharedData>("SharedData",
			"root", &SharedData::root,
			"print", &SharedData::print
		);

	mState["sharedData"] = global_sharedData;

	mState.new_usertype<DataNode>("DataNode",
			"at", &DataNode::at,
			"get", &DataNode::get,
			"set", sol::overload(
							static_cast<void (DataNode::*)(std::string)>(&DataNode::set),
							static_cast<void (DataNode::*)(int)>(&DataNode::set)
						),
			"branches", &DataNode::branches
		);
}

void Lua::setup()
{
	mState.set_panic(luaPanic);

	auto m = mState.script_file("lua_lib/main.lua");
	if(!m.valid())
		Log(Log::DIE) << "Lua: cannot load main.lua";

	auto g = mState.script_file(global_config->get("gameFile").c_str());
	if(!g.valid())
		Log(Log::DIE) << "Lua: cannot load " << global_config->get("gameFile");

	mState["setup"]();
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
		mState["main_loop"]();
}

void Lua::execute(const char *cmd)
{
	mState.script(cmd);
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
	mState["resizeWindow"]();
}

void Lua::sharedDataUpdated(const std::string &line)
{
	mState["sharedDataUpdated"](line);
}

