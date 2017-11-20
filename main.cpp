#include "window.h"
#include "log.h"
#include "selene.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
Window *ptr_w; // Ugly but works
void main_loop()
{
    ptr_w->onLoop();
}
#endif

int main( int argc, char* args[] )
{

sel::State state;
state.Load("assets/lua.lua");
Log() << int(state["foo"]);
state("x = 15");
Log() << int(state["x"]);

    Window w;

    #ifdef __EMSCRIPTEN__
        ptr_w = &w;
        Log() << "Running Emscripten loop";
        emscripten_set_main_loop(main_loop, 0, true);
    #else
        w.loop();
    #endif

    return 0;
}

