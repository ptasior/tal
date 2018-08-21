#include "game.h"
#include "log.h"
#include "global.h"

#ifdef JS
	#include <emscripten.h>
#endif

#ifdef JS
void main_loop()
{
	if(!Global::get<Game>()->loop())
		emscripten_cancel_main_loop();
}
#endif

int main(int argc, char* argv[])
{
	try
	{
		Log() << "Starting --------------";
		Global::init<Game>(new Game());

		Global::get<Game>()->init();

		#ifdef JS
			Log() << "Running Emscripten loop";
			emscripten_set_main_loop(main_loop, 0, true);
		#else
			Game *g = Global::get<Game>();
			while(g->loop())
				;
		#endif

		Log() << "Loop finished";
	}
	catch(const std::exception& e)
	{
		Log(Log::DIE) << e.what();
	}

	return 0;
}


