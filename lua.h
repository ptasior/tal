#pragma once

class Gui;
class Scene;
class Camera;

class Lua
{
public:
	void initGui(Gui *gui);
	void initScene(Scene *scene);
	void initCamera(Camera *camera);

	void run();

private:
	void applyWidgetInheritance(const char *type);
	Gui *mGui;
	Scene *mScene;
	Camera *mCamera;
};
