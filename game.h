#pragma once
// SDL and  string included in net.h
// #include "gl_header.h"
#include "game_file.h"
#include <vector>

class Scene;
class Camera;
class Net;
class Gui;
class Config;
class SharedData;

class Game
{
public:
	Game();
	~Game();

	void init();
	bool loop();

	std::shared_ptr<StreamReader> findGameFile(const std::string &name) const;

private:
	void populateData();

	std::vector<GameFile> mGameFiles;
};

