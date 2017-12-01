#pragma once

class Gui;

class Lua
{
public:
	void init(Gui *gui);
	void run();

private:
	void applyWidgetInheritance(const char *type);
	Gui *mGui;
};
