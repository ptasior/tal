#include "scene.h"
#include "shader.h"
#include "camera.h"
#include "sprite.h"
#include "model_cube.h"
#include "model_obj.h"
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
	Log() << "Scene Init";

	mUniformMVP = Shader::getShader("triangle")->mkUniform("mvp");

	// mModel.init();
	// mSprites.push_back(std::make_shared<Sprite>());
	// mSprites[0]->init("assets/tex.png", "triangle");
    //
	// glm::mat4 pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.0, 0.5));
	// mSprites[0]->setPosition(pos);
    //
	// mSprites.emplace(std::make_shared<Sprite>());
	// mSprites[0]->init("assets/penguin.png");

	mSprites.push_back(std::make_shared<Sprite>());
	mSprites[0]->init("assets/penguin.png", "triangle");

	glm::mat4 pos2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.8, 0.0)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
		// * glm::rotate(glm::mat4(1.0f), glm::radians(1.7f), axis_y);
	mSprites[0]->setPosition(pos2);

	mModels.push_back(std::make_shared<ModelObj>());
	// mModels.push_back(std::make_shared<ModelCube>());
	mModels[0]->init();

	glm::mat4 m = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	mModels[0]->setPosition(m);
}

void Scene::paint()
{
	float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
	glm::vec3 axis_y(0, 1, 0);
	glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

	mCamera->setPreRot(anim);
	mCamera->apply(mUniformMVP);



	for(auto m : mModels)
		m->paint();

	for(auto s : mSprites)
		s->paint();
}


void Scene::setCamera(std::shared_ptr<Camera> camera)
{
	mCamera = camera;
}
