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
#include "global.h"
#include "game.h"
#include "shared_data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Lua::logFnc(std::string s)
{
	Log() << "-- " << s;
	// Gui* gui = getInstance()->mGui;
	// if(gui) gui->getConsole()->log(s);
}

sol::object Lua::requireScript(std::string s)
{
	auto stream = Global::get<Game>()->openResource(s+".lua");
	std::replace(s.begin(), s.end(), '/', '_'); // Convert / to _  in name as it must be legit

	return Global::get<Lua>()->mState.require_script(s, stream->readToString());
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

Lua::Lua()
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


	mState["requireScript"] = &Lua::requireScript;
	mState["log"] = &Lua::logFnc;
	mState["wireframe"] = &Lua::wireframe;
	mState["setLoopResolution"] = &Lua::setLoopResolution;
	mState["setWait"] = &Lua::setWait;
	mState["setTimeout"] = &Lua::setTimeout;
	mState["execute"] = &Lua::executeLua;

	mState["updateAwaitingExecution"] = &Lua::updateAwaitingExecution;

	mState.new_usertype<SharedData>("SharedData",
			"root", &SharedData::root,
			"print", &SharedData::print
		);

	mState["sharedData"] = Global::get<SharedData>();

	mState.new_usertype<DataNode>("DataNode",
			"at", &DataNode::at,
			"get", &DataNode::get,
			"set", sol::overload(
							static_cast<void (DataNode::*)(std::string)>(&DataNode::set),
							static_cast<void (DataNode::*)(int)>(&DataNode::set)
						),
			"set_i", &DataNode::set_i,
			"branches", &DataNode::branches
		);
}

void Lua::setup()
{
	mState.set_panic(luaPanic);

	auto stream = Global::get<Game>()->openResource("lua_lib/main.lua");
	auto m = mState.script(stream->readToString());
	if(!m.valid())
		Log(Log::DIE) << "Lua: cannot load main.lua";

	stream = Global::get<Game>()->openResource(Global::get<Config>()->get("gameFile").c_str());
	auto g = mState.script(stream->readToString());
	if(!g.valid())
		Log(Log::DIE) << "Lua: cannot load " << Global::get<Config>()->get("gameFile");

	try
	{
		mState["setup"]();
	}
	catch(const sol::error& e)
	{
		lua_State *L = mState.lua_state();

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
					<< e.what()
					<< msg;
					// << sol::stack::get<std::string>(L, -1);
	}
	catch (const std::exception& e)
	{
		Log(Log::DIE) << "Generic exception: " << e.what();
	}
}

void Lua::loop()
{
	static unsigned int time = Global::get<Time>()->current();

	auto currentTime = Global::get<Time>()->current();
	if(time + mLoopResolution > currentTime)
		return; // Don't call every frame

	if(mWait == wsRefresh) // Run one frame
		mWait = wsRun;

	if(mWait == wsSleep && currentTime > mTimeout)
		mWait = wsRun;

	time = currentTime;

	if(mWait != wsRun)
		return;

	try
	{
		mState["main_loop"]();
	}
	catch(const sol::error& e)
	{
		lua_State *L = mState.lua_state();
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
					<< e.what()
					<< msg;
					// << sol::stack::get<std::string>(L, -1);
	}
	catch (const std::exception& e)
	{
		Log(Log::DIE) << "Generic exception: " << e.what();
	}
}

void Lua::execute(const char *cmd)
{
	Global::get<Lua>()->mExecuteStrings.push_back(cmd);
}

void Lua::executeLua(sol::function f)
{
	Global::get<Lua>()->mExecuteLuaFunctions.push_back(f);
}

void Lua::execute(sol::function f)
{
	Global::get<Lua>()->mExecuteLuaFunctions.push_back(f);
}

void Lua::execute(std::function<void(void)> f)
{
	Global::get<Lua>()->mExecuteFunctions.push_back(f);
}

void Lua::updateAwaitingExecution()
{
	Global::get<Lua>()->mState["global_toExecute"] = Global::get<Lua>()->mExecuteFunctions;
	Global::get<Lua>()->mState["global_toExecuteLua"] = Global::get<Lua>()->mExecuteLuaFunctions;
	Global::get<Lua>()->mState["global_toExecuteStrings"] = Global::get<Lua>()->mExecuteStrings;

	// TODO Check if it is OK or does it remove memory bbefore use...
	Global::get<Lua>()->mExecuteFunctions.clear();
	Global::get<Lua>()->mExecuteLuaFunctions.clear();
	Global::get<Lua>()->mExecuteStrings.clear();
}

void Lua::wireframe()
{
	static bool wf = false;
	wf = !wf;
#ifdef DESKTOP
	glPolygonMode(GL_FRONT_AND_BACK, wf?GL_LINE:GL_FILL);
#endif
}

void Lua::setWait(int v)
{
	Global::get<Lua>()->mWait = (WaitState)v;
}

void Lua::setTimeout(int v)
{
	Global::get<Lua>()->mTimeout = Global::get<Time>()->current() + v;
}

void Lua::setLoopResolution(unsigned int res)
{
	Global::get<Lua>()->mLoopResolution = res;
}

void Lua::resizeWindow()
{
	if(mState["resizeWindow"].valid())
		mState["resizeWindow"]();
}

void Lua::sharedDataUpdated(const std::string &line)
{
	execute(("main_sharedDataUpdate(\""+line+"\")").c_str());
}

