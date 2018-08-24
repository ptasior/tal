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

	// mState.new_usertype<glm::mat4>("Glm_Mat4",
	// 		sol::constructors<glm::mat4(float)>()
	// 	);
	// mState.new_usertype<glm::mat3>("Glm_Vec3",
	// 		sol::constructors<glm::vec3(double, double, double)>()
	// 	);

	mState.new_usertype<Matrix>("Matrix",
			"val", &Matrix::val,
			"rotate", &Matrix::rotate,
			"translate", &Matrix::translate,
			"scale", &Matrix::scale,
			"scaleVec", &Matrix::scaleVec
		);


	mState.new_usertype<Map>("Map",
			"init", &Map::init,
			"addTexture", &Map::addTexture,
			"setRect", &Map::setRect,
			"getAltitude", &Map::getAltitude
		);

	mState.new_usertype<Skybox>("Skybox",
			"init", &Skybox::init,
			"setTexture", &Skybox::setTexture
		);


	mState.new_usertype<Sprite>("Sprite",
			"init", &Sprite::init,
			"setPosition", &Sprite::setPosition,
			"setSize", &Sprite::setSize
		);

	mState.new_usertype<Model>("ModelObj",
			"setPosition", &Model::setPosition
		);
	mState.new_usertype<ModelObj>("ModelObj",
			"init", &ModelObj::init
		);

	mState["scene"] = scene;
	mState.new_usertype<Scene>("Scene",
			"getMap", &Scene::getMap,
			"getSkybox", &Scene::getSkybox,
			"addModel", &Scene::addModel<ModelObj>,
			"addSprite", &Scene::addSprite
		);
}

