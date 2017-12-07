#pragma once
#include "gl_header.h"
#include <memory>
#include <vector>

class Camera;
class Sprite;
class Model;
class Map;
class ModelObj;

class Scene
{
public:
	Scene();

	void init();
	void setCamera(std::shared_ptr<Camera> camera);

	void paint();

	// For Lua
	Map &getMap();
	void addSprite(Sprite* s);

	template<class T>
	void addModel(T* m);


private:
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Map> mMap;
	std::vector<std::shared_ptr<Sprite>> mSprites;
	std::vector<std::shared_ptr<Model>> mModels;
	std::vector<Sprite*> mLuaSprites;
	std::vector<Model*> mLuaModels;
};

#include "scene.hpp"

