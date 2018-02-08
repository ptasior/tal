#include "window.h"
#include "log.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Window *global_window;

#ifdef __EMSCRIPTEN__
void main_loop()
{
	global_window->onLoop();
}
#endif

int main(int argc, char* args[])
{
	try
	{
		Window w;
		global_window = &w;
		w.init();

		#ifdef __EMSCRIPTEN__
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


