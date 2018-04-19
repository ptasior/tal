#include "window.h"
#include "log.h"

#ifdef JS
	#include <emscripten.h>
#endif

#ifdef ANDROID
	#include <SDL.h>
	#include <unistd.h>
#endif

Window *global_window;

#ifdef JS
void main_loop()
{
	global_window->onLoop();
}
#endif

int main(int argc, char* argv[])
{
	Log() << "Starting --------------";

	#ifdef ANDROID
		chdir("/sdcard/tal/");
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
		Window w;
		global_window = &w;
		w.init();

		#ifdef JS
			Log() << "Running Emscripten loop";
			emscripten_set_main_loop(main_loop, 0, true);
		#else
			global_window->loop();
		#endif
	}
	catch(const std::exception& e)
	{
		Log(Log::DIE) << e.what();
	}

	return 0;
}


