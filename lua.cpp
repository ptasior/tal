#include "lua.h"
#include "log.h"
#include "gui.h"
#include "scene.h"
#include "sprite.h"
#include "model_obj.h"
#include "skybox.h"
#include "map.h"
#include "matrix.h"
#include "time.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Lua::logFnc(std::string s)
{
	Log() << "-- " << s;
	Gui* gui = getInstance()->mGui;
	if(gui) gui->getConsole()->log(s);
}

Lua* Lua::getInstance()
{
	static Lua l;
	return &l;
}

Lua::Lua():
	state(true)
{
	state["log"] = &Lua::logFnc;
	state["wireframe"] = &Lua::wireframe;
	state["setLoopResolution"] = &Lua::setLoopResolution;
	state["setWait"] = &Lua::setWait;
	state["setTimeout"] = &Lua::setTimeout;
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

void Lua::initGui(Gui *gui)
{
	mGui = gui;

	state["gui"].SetObj<Gui>(*gui,
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight,
			"message", &Gui::message,
			"showFps", &Gui::showFps
		);

	applyWidgetInheritance("Label");
	state["Label"].SetClass<Label, std::string>("setText", &Label::setText);

	applyWidgetInheritance("Edit");
	state["Edit"].SetClass<Edit, std::string>();//"setText", &Button::setText);

	applyWidgetInheritance("Button");
	state["Button"].SetClass<Button, std::string>();//"setText", &Button::setText);

	applyWidgetInheritance("Box");
	state["Box"].SetClass<Box, std::string>();//"setText", &Button::setText);

	applyWidgetInheritance("ButtonBox");
	state["ButtonBox"].SetClass<ButtonBox, std::string>(
			"addBottomButton", &ButtonBox::addForeignBottomButton
		);

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
			"setStretch", &Widget::setStretch,
			"setColor", &Widget::setColor,

			"onClickLua", &Widget::onClickLua,

			"addWidget", &Widget::addWidget<Widget>,
			"addLabel", &Widget::addWidget<Label>,
			"addEdit", &Widget::addWidget<Edit>,
			"addButton", &Widget::addWidget<Button>,
			"addBox", &Widget::addWidget<Box>,
			"addButtonBox", &Widget::addWidget<ButtonBox>,

			"removeWidget", &Widget::removeWidget<Widget>,
			"removeLabel", &Widget::removeWidget<Label>,
			"removeEdit", &Widget::removeWidget<Edit>,
			"removeButton", &Widget::removeWidget<Button>,
			"removeBox", &Widget::removeWidget<Box>,
			"removeButtonBox", &Widget::removeWidget<ButtonBox>
		);
}

void Lua::setup()
{
	state.HandleExceptionsWith([this](int, std::string msg, std::exception_ptr){
			if(mGui) mGui->getConsole()->log(msg);
			Log() << "Lua exception: " << msg;
		});

	state.Load("game/game.lua");

	state["setup"]();
}

void Lua::loop()
{
	static unsigned int time = Time::current();

	if(time + mLoopResolution > Time::current())
		return; // Don't call every frame

	if(mWait == wsRefresh) // Run one frame
		mWait = wsRun;

	if(mWait == wsSleep && Time::current() > mTimeout)
		mWait = wsRun;

	time = Time::current();

	if(mWait == wsRun)
		state["loop"]();
}

void Lua::execute(const char *cmd)
{
	state(cmd);
}

void Lua::wireframe()
{
	static bool wf = false;
	wf = !wf;
#ifndef __EMSCRIPTEN__
	glPolygonMode(GL_FRONT_AND_BACK, wf?GL_LINE:GL_FILL);
#endif
}

void Lua::setWait(int v)
{
	getInstance()->mWait = (WaitState)v;
}

void Lua::setTimeout(int v)
{
	getInstance()->mTimeout = Time::current() + v;
}

void Lua::setLoopResolution(unsigned int res)
{
	getInstance()->mLoopResolution = res;
}

void Lua::resizeWindow()
{
	state["resizeWindow"]();
}

