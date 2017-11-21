#include "window.h"
#include "log.h"
#include <SDL.h>


#ifdef __EMSCRIPTEN__
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL_opengles2.h>
#else
	#include <GL/glew.h>

	#include <SDL2/SDL.h>
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL2/SDL_opengl.h>
#endif


const GLchar* vertexSource =
    "attribute vec4 position;    \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vec4(position.xyz, 1.0);  \n"
    "}                            \n";
const GLchar* fragmentSource =
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 (1.0, 1.0, 0.0, 1.0 );\n"
    "}                                            \n";

	GLuint vao;
	GLuint vbo;
	GLuint shaderProgram;
	GLint posAttrib;
	GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

Window::Window()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if(SDL_Init( SDL_INIT_VIDEO ) < 0)
		Log(Log::DIE) << "SDL could not initialize! SDL_Error: " << SDL_GetError();

	//Create window
	mWindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(!mWindow)
		Log(Log::DIE) << "Window could not be created! SDL_Error: " << SDL_GetError();

	mGLContext = SDL_GL_CreateContext(mWindow);
	if(!mGLContext)
		Log(Log::DIE) << "GL context could not be created! SDL_Error: " << SDL_GetError();

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!mRenderer)
		Log(Log::DIE) << "Renderer could not be created! SDL_Error: " << SDL_GetError();

#ifdef __EMSCRIPTEN__
	glGenVertexArraysOES(1, &vao);
	glBindVertexArrayOES(vao);
#else
	glewExperimental=true; // Needed in core profile
	GLenum glew = glewInit();

	if(glew!= GLEW_OK)
		Log(Log::DIE) << "Failed to initialize GLEW";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif

	// Create a Vertex Buffer Object and copy the vertex data to it
	glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compile_ok);
	if(!compile_ok)
		Log(Log::DIE) << "Error in vertex shader";


    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compile_ok);
	if(!compile_ok)
		Log(Log::DIE) << "Error in fragment shader";


    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
		Log(Log::DIE) << "Error in glLinkProgram";

    // Specify the layout of the vertex data
    posAttrib = glGetAttribLocation(shaderProgram, "position");
	if(posAttrib == -1)
		Log(Log::DIE) << "Could not bind attribute";

	Log() << "Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	Log() << "Quit";
}

void Window::onEvent(SDL_Event &event)
{
	/* an event was found */
	switch (event.type)
	{
		case SDL_QUIT:
			mQuit = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
					mQuit = true;
					break;
			case SDLK_p:
					Log() << "a log";
					break;
			case SDLK_n:
					Log() << "net";
					mNet.connect();
					mNet.send("new message");
					break;
			}
			break;
	}
}

void Window::loop()
{
	while (!mQuit)
		onLoop();
}

void Window::onLoop()
{
	static SDL_Event event; // This SHOULD be safe

	if(SDL_PollEvent(&event))
		onEvent(event);

	mNet.loop();
	onPaint();
}

void Window::onPaint()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw a triangle from the 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(posAttrib);

	SDL_GL_SwapWindow(mWindow);
}

