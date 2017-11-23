#pragma once
#include <memory>
#include <vector>

class Camera;
class Sprite;

class Scene
{
public:
	Scene();

	void init();
	void setSceneSize(int w, int h);

	void paint();

private:
	std::shared_ptr<Camera> mCamera;
	std::vector<std::shared_ptr<Sprite>> mSprites;

	int mSceneWidth;
	int mSceneHeight;
};

