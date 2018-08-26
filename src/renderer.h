#pragma once
#include <map>
#include <memory>

class Camera;
class Scene;
class Gui;
class Shader;

class Renderer
{
public:
	void init();
	void initCamera(); //const

	void windowResized(int width, int height);
	void paint(); //const

	Camera* getCamera();
	Scene* getScene();
	Gui* getGui();
	std::shared_ptr<Shader> shader(const char *name);

private:
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<Gui> mGui;

	std::map<std::string, std::shared_ptr<Shader>> mShaders;
};

