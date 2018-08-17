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
		return instance<T>.get();
	}

	template<class T>
    static void init(T* t)
	{
		if(instance<T>)
			Log(Log::DIE) << "Global: type already initialised";

		instance<T>.reset(t);
	}

	Global(const Global&) = delete;
	Global& operator= (const Global) = delete;

protected:
    Global() {};

	template<class T>
	static std::unique_ptr<T> instance;
};

