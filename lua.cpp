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
#include "config.h"
#include "shared_data.h"

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

int luaPanic(lua_State* s)
{
	// if(mGui) mGui->getConsole()->log(msg);
	std::string msg = sol::stack::get<std::string>(s, -1);

	Log(Log::DIE)<< "Lua exception: " << msg;
	return -1;
}

Lua::Lua():
	mState(sol::c_call<decltype(&luaPanic), &luaPanic>)
{
	mState.open_libraries(sol::lib::base,
						sol::lib::package,
						sol::lib::coroutine,
						sol::lib::string,
						sol::lib::os,
						sol::lib::math,
						sol::lib::table,
						sol::lib::debug
					);


	mState["log"] = &Lua::logFnc;
	mState["wireframe"] = &Lua::wireframe;
	mState["setLoopResolution"] = &Lua::setLoopResolution;
	mState["setWait"] = &Lua::setWait;
	mState["setTimeout"] = &Lua::setTimeout;

	mState.new_usertype<SharedData>("SharedData",
			"root", &SharedData::root,
			"print", &SharedData::print
		);

	mState["sharedData"] = *global_sharedData;

	mState.new_usertype<DataNode>("DataNode",
			"at", &DataNode::at,
			"get", &DataNode::get,
			"set", &DataNode::set,
			"branches", &DataNode::branches
		);
}

void Lua::setup()
{
	mState.set_panic(luaPanic);

	// auto m = mState.load_file("lua_lib/main.lua");
	// if(!m.valid())
	// 	Log(Log::DIE) << "Lua: cannot load main.lua";

	auto g = mState.script_file(global_config->get("gameFile").c_str());
	if(!g.valid())
		Log(Log::DIE) << "Lua: cannot load " << global_config->get("gameFile");
	// m();
	// g();
	Log() << "here";

	sol::function stp = mState["setup"];
	if(!stp.valid())
		Log(Log::DIE) << "invalid function";
	stp();
	Log() << "here";
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
		mState["main_loop"]();
}

void Lua::execute(const char *cmd)
{
	mState.script(cmd);
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
	mState["resizeWindow"]();
}

void Lua::sharedDataUpdated(const std::string &line)
{
	mState["sharedDataUpdated"](line);
}

void Lua::initGui(Gui *gui)
{
	mGui = gui;

	mState["gui"] = *gui;
	mState.new_usertype<Gui>("Gui",
			"rootWidget", &Gui::rootWidget,
			"getSceneWidth", &Gui::getSceneWidth,
			"getSceneHeight", &Gui::getSceneHeight,
			"message", &Gui::message,
			"showFps", &Gui::showFps
		);

	applyWidgetInheritance("MultiLine");
	mState.new_usertype<MultiLine>("MultiLine",
					sol::constructors<MultiLine(std::string)>(),
					"setText", &MultiLine::setText,
					"resize", &MultiLine::resize,
					"label", &MultiLine::label,
					"linesCount", &MultiLine::linesCount
				);

	applyWidgetInheritance("Label");
	mState.new_usertype<Label>("Label",
					sol::constructors<Label(std::string)>(),
					"setText", &Label::setText,
					"getText", &Label::getText
				);

	applyWidgetInheritance("Edit");
	mState.new_usertype<Edit>("Edit",
					sol::constructors<Edit(std::string)>(),
					"setText", &Edit::setText,
					"getText", &Edit::getText
				);

	applyWidgetInheritance("Button");
	mState.new_usertype<Button>("Button",
					sol::constructors<Button(std::string)>(),
					"setText", &Button::setText
				);

	applyWidgetInheritance("Box");
	mState.new_usertype<Box>("Box",
					sol::constructors<Box(std::string)>()
				);

	applyWidgetInheritance("Scroll");
	mState.new_usertype<Scroll>("Scroll",
					"clear", &Scroll::clear
				);

	applyWidgetInheritance("Checkbox");
	mState.new_usertype<Checkbox>("Checkbox",
					sol::constructors<Checkbox(bool)>(),
					"setChecked", &Checkbox::setChecked,
					"isChecked", &Checkbox::isChecked
				);

	applyWidgetInheritance("ButtonBox");
	mState.new_usertype<ButtonBox>("ButtonBox",
					sol::constructors<ButtonBox(std::string)>(),
					"addBottomButton", &ButtonBox::addForeignBottomButton
				);

	applyWidgetInheritance("Widget");
}


void Lua::applyWidgetInheritance(const char *type)
{
	mState.new_usertype<Widget>("Widget",
			sol::constructors<Widget(std::string)>(),
			"setTop", &Widget::setTop,
			"setLeft", &Widget::setLeft,
			"setWidth", &Widget::setWidth,
			"setHeight", &Widget::setHeight,
			"setPosition", &Widget::setPosition,
			"setSize", &Widget::setSize,
			"setRect", &Widget::setRect,
			"setPadding", &Widget::setPadding,
			"setLayout", &Widget::setLayout,
			"setOverflow", &Widget::setOverflow,
			"setCenter", &Widget::setCenter,
			"setStretch", &Widget::setStretch,
			"setColor", &Widget::setColor,

			"onClickLua", &Widget::onClickLua,

			"getTop", &Widget::getTop,
			"getLeft", &Widget::getLeft,
			"getWidth", &Widget::getWidth,
			"getHeight", &Widget::getHeight,

			"addWidget", &Widget::addWidget<Widget>,
			"addLabel", &Widget::addWidget<Label>,
			"addMultiLine", &Widget::addWidget<MultiLine>,
			"addEdit", &Widget::addWidget<Edit>,
			"addButton", &Widget::addWidget<Button>,
			"addBox", &Widget::addWidget<Box>,
			"addButtonBox", &Widget::addWidget<ButtonBox>,
			"addScroll", &Widget::addWidget<Scroll>,
			"addCheckbox", &Widget::addWidget<Checkbox>,

			"removeWidget", &Widget::removeWidget<Widget>,
			"removeLabel", &Widget::removeWidget<Label>,
			"removeMultiLine", &Widget::removeWidget<MultiLine>,
			"removeEdit", &Widget::removeWidget<Edit>,
			"removeButton", &Widget::removeWidget<Button>,
			"removeBox", &Widget::removeWidget<Box>,
			"removeButtonBox", &Widget::removeWidget<ButtonBox>,
			"removeScroll", &Widget::removeWidget<Scroll>
		);
}

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

	mState["scene"] = *scene;
	mState.new_usertype<Scene>("Scene",
			"getMap", &Scene::getMap,
			"getSkybox", &Scene::getSkybox,
			"addModel", &Scene::addModel<ModelObj>,
			"addSprite", &Scene::addSprite
		);
}

