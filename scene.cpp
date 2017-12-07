#include "scene.h"
#include "shader.h"
#include "camera.h"
#include "sprite.h"
#include "model_cube.h"
#include "model_obj.h"
#include "map.h"
#include "log.h"
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
{
}

void Scene::init()
{
	Log() << "Scene: Init";

	GLuint mUniformMVP = Shader::getShader("model")->mkUniform("mvp");

	// Setup camera for shader
	Shader::getShader("model")->setOnChange([this, mUniformMVP](){
			mCamera->apply(mUniformMVP);
		});

	Shader::getShader("triangle")->setOnChange([this, mUniformMVP](){
			mCamera->applySprite(mUniformMVP);
		});

	mMap = std::make_shared<Map>();
}

void Scene::paint()
{
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
}

Map &Scene::getMap()
{
	return *mMap.get();
}

void Scene::addSprite(Sprite* s)
{
	mLuaSprites.push_back(s);
}

