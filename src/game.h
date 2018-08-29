#pragma once
// SDL and  string included in net.h
// #include "gl_header.h"
#include "window.h"
#include "game_file.h"
#include <vector>

class StreamReader;
class Net;
class SharedData;

class Game
{
public:
	Game();
	~Game();

	void init();
	bool loop();

	std::shared_ptr<StreamReader> openResource(const std::string &name) const;
	bool hasResource(const std::string &name) const;

private:
	void loadGameFiles();
	void loadConfig();
	void mainMenu();

	std::vector<GameFile> mGameFiles;
	std::shared_ptr<Window> mWindow;
	std::shared_ptr<Net> mNet;
	std::shared_ptr<SharedData> mSharedData;
};

