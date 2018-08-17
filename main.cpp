#include "window.h"
#include "log.h"
#include "config.h"
#include "global.h"

#ifdef JS
	#include <emscripten.h>
#endif

#ifdef ANDROID
	#include <SDL.h>
	#include <unistd.h>
#endif

#ifdef JS
void main_loop()
{
	Global::get<Window>()->onLoop();
}
#endif

int main(int argc, char* argv[])
{
	Log() << "Starting --------------";

	#ifdef ANDROID
		// chdir(ANDROID_DATA_PATH);
	#endif

	// DIR           *d;
	// struct dirent *dir;
	// d = opendir(".");
    //
	// if (d)
	// {
	// 	while ((dir = readdir(d)) != NULL)
	// 		Log() << dir->d_name;
    //
	// 	closedir(d);
	// }

	try
	{
		Global::init<Window>(new Window());
		Global::get<Window>()->init();

		#ifdef JS
			Log() << "Running Emscripten loop";
			emscripten_set_main_loop(main_loop, 0, true);
		#else
			Global::get<Window>()->loop();
		#endif
	}
	catch(const std::exception& e)
	{
		Log(Log::DIE) << e.what();
	}

	return 0;
}


