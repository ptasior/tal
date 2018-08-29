#include "game.h"
#include "log.h"
#include "net.h"
// #include "shader.h"
// #include "camera_rotating.h"
// #include "camera_fps.h"
// #include "scene.h"
#include "gui.h"
#include "lua.h"
#include "renderer.h"
#include "time.h"
#include "config.h"
#include "global.h"
#include "shared_data.h"
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
	Global::init<Config>(new Config());
	Global::init<Time>(new Time());
	Global::init<Renderer>(new Renderer());
	Global::init<SharedData>(new SharedData());
	Global::init<Lua>(new Lua());

	loadGameFiles();
	loadConfig();

	mWindow = std::make_shared<Window>();
	mWindow->init();

	Global::get<Renderer>()->init();
	mWindow->updateSize();

	mNet = std::make_shared<Net>();

	// TODO How to make sure we have received self registration before entering Lua?
	if(!Global::get<Config>()->getBool("offline"))
		mNet->connect();

	Global::get<Lua>()->initGui(Global::get<Renderer>()->gui());
	Global::get<Lua>()->initScene(Global::get<Renderer>()->scene());
	Global::get<Lua>()->setup();

	Log() << "Game: Initialisation succesed";

	mainMenu();

	if(Global::get<Config>()->getInt("loopSleep"))
		Log() << "Game: Entering throttled loop sleep: " << Global::get<Config>()->get("loopSleep");
}

Game::~Game()
{}

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

bool Game::hasResource(const std::string &name) const
{
	for(auto f : mGameFiles)
		if(f.hasFile(name))
			return true;

	return false;
}

void Game::loadConfig()
{
	auto r = openResource("defaults/config.cfg");
	Global::get<Config>()->load(r);
	Log() << "Game: Loaded default config";

	for(auto f : mGameFiles)
		if(f.hasFile("config.cfg"))
		{
			auto streamreader = f.readStream("config.cfg");
			Global::get<Config>()->load(streamreader);
			Log() << "Game: Config loaded from: " << f.name();
		}

	if(FileUtils::fileExists("config.cfg"))
	{
		std::shared_ptr<StreamReader> file = std::make_shared<StreamReader>();
		file->openFile("config.cfg");
		Global::get<Config>()->load(file);
		Log() << "Game: Config loaded from disk file";
	}

	Global::get<Config>()->print();
}

void Game::mainMenu()
{
	auto renderer = Global::get<Renderer>();

	renderer->gui()->message("test", "This is a test");
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

	mNet->loop();
	// Internally executes every n-th frame
	Global::get<Lua>()->loop();

	return mWindow->loop();
}

