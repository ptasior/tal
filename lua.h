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

	void run();
	static Lua* getInstance();

private:
	Lua();
	void applyWidgetInheritance(const char *type);

	static void logFnc(std::string s);
	static void wireframe();

	Gui *mGui;
	Scene *mScene;

	sel::State state;
};
