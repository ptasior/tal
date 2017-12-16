#pragma once
#include <selene.h>

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

	void setup();
	void loop();
	void resizeWindow();
	static Lua* getInstance();

private:
	Lua();
	void applyWidgetInheritance(const char *type);

	static void logFnc(std::string s);
	static void wireframe();

	Gui *mGui;
	Scene *mScene;
	unsigned int mLoopResolution = 10;

	sel::State state;
};
