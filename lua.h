#pragma once
#include <selene.h>

class Gui;
class Scene;
class Camera;

class Lua
{
public:
	void initGui(Gui *gui);
	void initScene(Scene *scene);
	void initCamera(Camera *camera);
	void execute(const char *cmd);

	void run();
	static Lua* getInstance();

private:
	Lua();
	void applyWidgetInheritance(const char *type);

	static void logFnc(std::string s);

	Gui *mGui;
	Scene *mScene;
	Camera *mCamera;

	sel::State state;
};
