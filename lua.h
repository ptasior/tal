#pragma once
#include <sol.hpp>

class Gui;
class Scene;

class Lua
{
public:
	void initGui(Gui *gui);
	void initScene(Scene *scene);
	static void execute(const char *cmd);
	static void execute(sol::function f);
	static void executeLua(sol::function f);
	static void execute(std::function<void(void)> f);

	// TODO Make it non static and provide Lua obj to script
	static void setLoopResolution(unsigned int res);
	static void setWait(int v);
	static void setTimeout(int v);

	void setup();
	void loop();
	void resizeWindow();
	void sharedDataUpdated(const std::string &line);
	static Lua* getInstance();

private:
	enum WaitState {wsRun, wsWait, wsRefresh, wsSleep};

	Lua();

	static void updateAwaitingExecution();
	static void logFnc(std::string s);
	static void wireframe();

	Gui *mGui;
	Scene *mScene;
	unsigned int mLoopResolution = 10;
	WaitState mWait = wsRun;
	int mTimeout;

	std::vector<std::string> mExecuteStrings;
	std::vector<sol::function> mExecuteLuaFunctions;
	std::vector<std::function<void(void)>> mExecuteFunctions;

	sol::state mState;
};

