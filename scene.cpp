#include "scene.h"
#include "shader.h"
#include "camera.h"
#include "sprite.h"
#include "skybox.h"
#include "model_obj.h"
#include "map.h"
#include "log.h"
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
{
}

void Scene::init()
{
	Log() << "Scene: Init";

	mSkybox = std::make_shared<Skybox>();
	mMap = std::make_shared<Map>();
}

void Scene::paint()
{
	mCamera->apply();

	mSkybox->paint();
	mMap->paint();

	for(auto m : mModels)
		m->paint();

	for(auto m : mLuaModels)
		m->paint();

	for(auto s : mSprites)
		s->paint();

	for(auto s : mLuaSprites)
		s->paint();
}

void Scene::setCamera(std::shared_ptr<Camera> camera)
{
	mCamera = camera;
	// mCamera->apply();
}

Map &Scene::getMap()
{
	return *mMap.get();
}

Skybox &Scene::getSkybox()
{
	return *mSkybox.get();
}

void Scene::addSprite(Sprite* s)
{
	mLuaSprites.push_back(s);
}

