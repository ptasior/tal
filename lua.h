#pragma once

class Gui;

class Lua
{
public:
	void init(Gui *gui);
	void run();

private:
	Gui *mGui;

	// sel::State& operator();
};
