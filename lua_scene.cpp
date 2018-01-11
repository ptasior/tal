#include "lua.h"
#include "log.h"
#include "scene.h"
#include "sprite.h"
#include "model_obj.h"
#include "skybox.h"
#include "map.h"
#include "matrix.h"
#include "time.h"
#include "config.h"
#include "shared_data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Lua::initScene(Scene *scene)
{
	mScene = scene;

	state["Glm_Mat4"].SetClass<glm::mat4, float>();
	state["Glm_Vec3"].SetClass<glm::vec3, double, double, double>();

	state["Matrix"].SetClass<Matrix>(
			"val", &Matrix::val,
			"rotate", &Matrix::rotate,
			"translate", &Matrix::translate,
			"scale", &Matrix::scale,
			"scaleVec", &Matrix::scaleVec
		);


	state["Map"].SetClass<Map>(
			"init", &Map::init,
			"addTexture", &Map::addTexture,
			"setRect", &Map::setRect,
			"getAltitude", &Map::getAltitude
		);

	state["Skybox"].SetClass<Skybox>(
			"init", &Skybox::init,
			"setTexture", &Skybox::setTexture
		);


	state["Sprite"].SetClass<Sprite>(
			"init", &Sprite::init,
			"setPosition", &Sprite::setPosition,
			"setSize", &Sprite::setSize
		);

	state["ModelObj"].SetClass<Model>(
			"setPosition", &Model::setPosition
		);
	state["ModelObj"].SetClass<ModelObj>(
			"init", &ModelObj::init
		);

	state["scene"].SetObj<Scene>(*scene,
			"getMap", &Scene::getMap,
			"getSkybox", &Scene::getSkybox,
			"addModel", &Scene::addModel<ModelObj>,
			"addSprite", &Scene::addSprite
		);
}

