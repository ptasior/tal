#pragma once
// SDL and  string included in net.h
// #include "gl_header.h"
#include "window.h"
#include "game_file.h"
#include <vector>

class Scene;
class Camera;
class Net;
class Gui;
class Config;
class SharedData;
class Window;

class Game
{
public:
	Game();
	~Game();

	void init();
	bool loop();

	std::shared_ptr<StreamReader> openResource(const std::string &name) const;

private:
	void loadGameFiles();
	void loadConfig();

	std::vector<GameFile> mGameFiles;
	std::shared_ptr<Window> mWindow;
};

