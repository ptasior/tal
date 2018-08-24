#include "game.h"
#include "log.h"
// #include "net.h"
// #include "shader.h"
// #include "camera_rotating.h"
// #include "camera_fps.h"
// #include "scene.h"
#include "gui.h"
// #include "lua.h"
#include "time.h"
#include "config.h"
#include "global.h"
// #include "shared_data.h"
#include "game_file.h"
#include "file_utils.h"
#include <thread>
#include <chrono>

#include "embedded_defaults.h"

#ifdef DESKTOP
	#include <SDL2/SDL_ttf.h>
#else
	#include <SDL_ttf.h>
#endif

Game::Game()
{}

void Game::init()
{
	loadGameFiles();

	loadConfig();


	Global::init<Time>(new Time());

	mWindow = std::make_shared<Window>();
	mWindow->init();
	mWindow->initObjects();

	// mSharedData = std::make_shared<SharedData>();
	// global_sharedData = mSharedData.get();
    //
	// mNet = std::make_shared<Net>();
    //
	// // TODO How to make sure we have received self registration before entering Lua?
	// if(!mConfig->getBool("offline"))
	// 	mNet->connect();
    //
	// Lua::getInstance()->initGui(mGui.get());
	// Lua::getInstance()->initScene(mScene.get());
	// Lua::getInstance()->setup();

	Log() << "Game: Initialisation succesed";

	mainMenu();

	if(Global::get<Config>()->getInt("loopSleep"))
		Log() << "Game: Entering throttled loop sleep: " << Global::get<Config>()->get("loopSleep");
}

void Game::loadGameFiles()
{
	mGameFiles.clear();

	std::vector<std::string> gameFiles = FileUtils::listDir(FileUtils::dataFolderLocation(), ".game");
	for(auto f : gameFiles)
	{
		Log() << "Game: Loading file: " << f;
		std::shared_ptr<StreamReader> file = std::make_shared<StreamReader>();
		file->openFile(f);
		mGameFiles.emplace_back(GameFile(file));
	}

	Log() << "Game: Loading embedded defaults";
	std::shared_ptr<StreamReader> embedded = std::make_shared<StreamReader>();
	embedded->openString((const char*)EmbeddedData::defaultFiles, 0, EmbeddedData::defaultFiles_size);
	mGameFiles.emplace_back(GameFile(embedded));
}

std::shared_ptr<StreamReader> Game::openResource(const std::string &name) const
{
	for(auto f : mGameFiles)
		if(f.hasFile(name))
			return f.readStream(name);

	Log(Log::DIE) << "Game: Could not find file: " << name;
	return std::shared_ptr<StreamReader>();
}

void Game::loadConfig()
{
	Global::init<Config>(new Config());
	auto r = openResource("defaults/config.cfg");
	Global::get<Config>()->load(r);

	for(auto f : mGameFiles)
		if(f.hasFile("config.cfg"))
		{
			auto streamreader = f.readStream("config.cfg");
			Global::get<Config>()->load(streamreader);
		}
	// Global::get<Config>->print();
}

void Game::mainMenu()
{
	mWindow->getGui()->message("test", "This is a test");
	// mWindow->getGui()->rootWidget()->addWidget(m);
}

bool Game::loop()
{
#if ! defined(ANDROID) // Throttled loop crashes Android
	if(Global::get<Config>()->has("loopSleep")) // TODO Move it ou tof loop
	{
		int sleep = Global::get<Config>()->getInt("loopSleep");
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
	}
#endif

	return mWindow->onLoop();
}

Game::~Game()
{
}

