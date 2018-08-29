#pragma once
#include <map>
#include <memory>

class Camera;
class Scene;
class Gui;
class Shader;
class Texture;

class Renderer
{
public:
	void init();
	void initCamera(); //const

	void windowResized(int width, int height);
	void paint(); //const

	Camera* camera();
	Scene* scene();
	Gui* gui();
	std::shared_ptr<Shader> shader(const char *name);
	std::shared_ptr<Texture> texture(const char* path, Shader *s, const char* name=nullptr, int id=0);

private:
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<Gui> mGui;

	std::map<std::string, std::shared_ptr<Shader>> mShaders;
	std::map<std::string, std::shared_ptr<Texture>> mTextures;
};

