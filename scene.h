#pragma once
#include "gl_header.h"
#include <memory>
#include <vector>

class Camera;
class Sprite;

class Scene
{
public:
	Scene();

	void init();
	void setCamera(std::shared_ptr<Camera> camera);

	void paint();

private:
	std::shared_ptr<Camera> mCamera;
	std::vector<std::shared_ptr<Sprite>> mSprites;
	GLuint mUniformMVP;
};

