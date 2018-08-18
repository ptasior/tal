#include "game.h"
#include "log.h"
// #include "net.h"
// #include "shader.h"
// #include "camera_rotating.h"
// #include "camera_fps.h"
// #include "scene.h"
// #include "gui.h"
// #include "lua.h"
// #include "time.h"
#include "config.h"
#include "global.h"
// #include "shared_data.h"
#include "game_file.h"
#include "file_utils.h"
#include <thread>
#include <chrono>

#include "output/embedded_defaults.h"

#ifdef DESKTOP
	#include <SDL2/SDL_ttf.h>
#else
	#include <SDL_ttf.h>
#endif

Game::Game()
{}

void Game::init()
{
	populateData();

	Config c;
	auto r = findGameFile("defaults/config.cfg");
	c.load(r);
	c.print();




	// TODO Make it configurable / read from config
	// Global::init<DataReader>(new DataReader(FileUtils::dataFolderLocation()+"/data.game"));
    //
	// mConfig = std::make_shared<Config>();
	// global_config = mConfig.get();
    //
	// Global::init<Window>(new Window());
	// Global::get<Window>()->init();
    //
	// mSharedData = std::make_shared<SharedData>();
	// global_sharedData = mSharedData.get();
    //
	// mNet = std::make_shared<Net>();
    //
	// // TODO How to make sure we have received self registration before entering Lua?
	// if(!mConfig->getBool("offline"))
	// 	mNet->connect();
    //
	// mCamera = std::make_shared<RotatingCamera>();
	// // mCamera = std::make_shared<FpsCamera>();
	// mCamera->init();
	// mCamera->setSceneSize(mScreenWidth, mScreenHeight);
    //
	// mScene = std::make_shared<Scene>();
	// mScene->init();
	// mScene->setCamera(mCamera);
    //
	// mGui = std::make_shared<Gui>();
	// mGui->init();
	// mGui->setSceneSize(mScreenWidth, mScreenHeight);
    //
	// Lua::getInstance()->initGui(mGui.get());
	// Lua::getInstance()->initScene(mScene.get());
	// Lua::getInstance()->setup();
    //
	Log() << "Window: Initialisation succesed";
}

void Game::populateData()
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

	Log() << "Game: Loading defaults";
	std::shared_ptr<StreamReader> embedded = std::make_shared<StreamReader>();
	embedded->openString(EmbeddedData::defaultFiles, 0, EmbeddedData::defaultFiles_size);
	mGameFiles.emplace_back(GameFile(embedded));
}

std::shared_ptr<StreamReader> Game::findGameFile(const std::string &name) const
{
	for(auto f : mGameFiles)
		if(f.hasFile(name))
			return f.readStream(name);

	Log(Log::DIE) << "Game: Could not find file: " << name;
	return std::shared_ptr<StreamReader>();
}

bool Game::loop()
{
	return false;
	// Global::get<Window>()->onLoop();
	return true;
}

Game::~Game()
{
}

