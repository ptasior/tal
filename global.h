/*
 * Global::init<Window>(Window());
 * Global::get<Window>()->init();
*/
#pragma once
#include "log.h"

class Global
{
public:
	template<class T>
    static T* get()
	{
		return getInstance<T>().get();
	}

	template<class T>
    static void init(T* t)
	{
		if(getInstance<T>())
			Log(Log::DIE) << "Global: type already initialised";

		getInstance<T>().reset(t);
	}

	Global(const Global&) = delete;
	Global& operator= (const Global) = delete;

protected:
    Global() {};

	template<class T>
	static std::unique_ptr<T>& getInstance()
	{
		static std::unique_ptr<T> instance;
		return instance;
	}
};

