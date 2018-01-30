#pragma once
#include <sol.hpp>

class Gui;
class Scene;

class Lua
{
public:
	void initGui(Gui *gui);
	void initScene(Scene *scene);
	void execute(const char *cmd);

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

	static void logFnc(std::string s);
	static void wireframe();

	Gui *mGui;
	Scene *mScene;
	unsigned int mLoopResolution = 10;
	WaitState mWait = wsRun;
	int mTimeout;

	sol::state mState;
};

