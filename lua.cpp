#include "lua.h"
#include "log.h"
#include "gui.h"
#include "scene.h"
#include "sprite.h"
#include "model_obj.h"
#include "skybox.h"
#include "map.h"
#include "matrix.h"
#include <selene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO Fix me somehow, I'm ugly
sel::State state{true};

void logFnc(std::string s)
{
	Log() << "-- " << s;
}

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
			"setRect", &Map::setRect
		);

	state["Skybox"].SetClass<Skybox>(
			"init", &Skybox::init,
			"setTexture", &Skybox::setTexture
		);


	state["Sprite"].SetClass<Sprite>(
			"init", &Sprite::init,
			"setPosition", &Sprite::setPosition
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

void Lua::initCamera(Camera *camera)
{
	mCamera = camera;
}

void Lua::initGui(Gui *gui)
{
	mGui = gui;

	state["log"] = &logFnc;
	state["gui"].SetObj<Gui>(*gui,
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight
		);

	applyWidgetInheritance("Label");
	state["Label"].SetClass<Label, std::string>("setText", &Label::setText);

	applyWidgetInheritance("Button");
	state["Button"].SetClass<Button, std::string>();//"setText", &Button::setText);

	applyWidgetInheritance("Box");
	state["Box"].SetClass<Box, std::string>();//"setText", &Button::setText);
	// state["Box"].SetClass<Box>();
	applyWidgetInheritance("Widget");
}

void Lua::applyWidgetInheritance(const char *type)
{
	state[type].SetClass<Widget, std::string>(
			"setTop", &Widget::setTop,
			"setLeft", &Widget::setLeft,
			"setWidth", &Widget::setWidth,
			"setHeight", &Widget::setHeight,
			"setPosition", &Widget::setPosition,
			"setSize", &Widget::setSize,
			"setRect", &Widget::setRect,
			"setLayout", &Widget::setLayout,
			"setOverflow", &Widget::setOverflow,
			"setCenter", &Widget::setCenter,
			"setColor", &Widget::setColor,

			"onClickLua", &Widget::onClickLua,

			"addWidget", &Widget::addWidget<Widget>,
			"addLabel", &Widget::addWidget<Label>,
			"addButton", &Widget::addWidget<Button>,
			"addBox", &Widget::addWidget<Box>,

			"removeWidget", &Widget::removeWidget<Widget>,
			"removeLabel", &Widget::removeWidget<Label>,
			"removeButton", &Widget::removeWidget<Button>,
			"removeBox", &Widget::removeWidget<Box>
		);
}

void Lua::run()
{
	state.HandleExceptionsWith([](int, std::string msg, std::exception_ptr){
			Log(Log::DIE) << "Lua exception: " << msg;
		});

	state.Load("assets/lua.lua");

	state["setupGui"]();
	state["setupCamera"]();
	state["setupScene"]();
}

