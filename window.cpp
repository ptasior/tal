#include "window.h"
#include "log.h"
#include <SDL.h>


#ifdef __EMSCRIPTEN__
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL_opengles2.h>
#else
	// #include <GL/gl.h>
	#include <SDL2/SDL.h>

	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL2/SDL_opengles2.h>
#endif


// const GLchar* vertexSource =
//     "attribute vec4 position;    \n"
//     "void main()                  \n"
//     "{                            \n"
//     "   gl_Position = vec4(position.xyz, 1.0);  \n"
//     "}                            \n";
// const GLchar* fragmentSource =
//     "precision mediump float;\n"
//     "void main()                                  \n"
//     "{                                            \n"
//     "  gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );\n"
//     "}                                            \n";
// 

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

	// // Create Vertex Array Object
	// GLuint vao;
	// glGenVertexArraysOES(1, &vao);
	// glBindVertexArrayOES(vao);

	// GLuint vbo;
	// glGenBuffers(1, &vbo);

	// GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

	// glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// // Create and compile the vertex shader
	// GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// glCompileShader(vertexShader);

	// // Create and compile the fragment shader
	// GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// glCompileShader(fragmentShader);

	// // Link the vertex and fragment shader into a shader program
	// GLuint shaderProgram = glCreateProgram();
	// glAttachShader(shaderProgram, vertexShader);
	// glAttachShader(shaderProgram, fragmentShader);
	// // glBindFragDataLocation(shaderProgram, 0, "outColor");
	// glLinkProgram(shaderProgram);
	// glUseProgram(shaderProgram);

	// // Specify the layout of the vertex data
	// GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	// glEnableVertexAttribArray(posAttrib);
	// glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);




	// // TODO Rubbish, remove later
	// std::string imagePath = "assets/bmp.bmp";
	// SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
	// if (bmp == nullptr){
	// 	SDL_DestroyRenderer(mRenderer);
	// 	SDL_DestroyWindow(mWindow);
	// 	Log() << "SDL_LoadBMP Error: " << SDL_GetError();
	// 	SDL_Quit();
	// 	return ;
	// }

	// tex = SDL_CreateTextureFromSurface(mRenderer, bmp);
	// SDL_FreeSurface(bmp);
	// if (tex == nullptr){
	// 	SDL_DestroyRenderer(mRenderer);
	// 	SDL_DestroyWindow(mWindow);
	// 	Log()<< "SDL_CreateTextureFromSurface Error: " << SDL_GetError();
	// 	SDL_Quit();
	// 	return ;
	// }

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
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw a triangle from the 3 vertices
	// glDrawArrays(GL_TRIANGLES, 0, 3);

	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glLoadIdentity();//load identity matrix
	// 
	// // glTranslatef(0.0f,0.0f,-4.0f);//move forward 4 units
	// 
	// glColor3f(0.0f,0.0f,1.0f); //blue color
	// 
	// glBegin(GL_LINE_LOOP);//start drawing a line loop
	//   glVertex3f(-1.0f,0.0f,0.0f);//left of window
	//   glVertex3f(0.0f,-1.0f,0.0f);//bottom of window
	//   glVertex3f(1.0f,0.0f,0.0f);//right of window
	//   glVertex3f(0.0f,1.0f,0.0f);//top of window
	// glEnd();//end drawing of line loop

	SDL_GL_SwapWindow(mWindow);
}

