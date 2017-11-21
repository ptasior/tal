#include "window.h"
#include "log.h"
#include <SDL.h>
#include <fstream>

GLuint shaderProgram;
GLint attribute_coord3d, attribute_v_color, uniform_fade;

struct attributes
{
	GLfloat coord3d[3];
	GLfloat v_color[3];
};
Window::Window()
{
struct attributes triangle_attributes[] = {
		{{ 0.0,  0.8, 0.0}, {1.0, 1.0, 0.0}},
		{{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
		{{ 0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}}
	};

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1); // Transparency

#ifndef __EMSCRIPTEN__
	glewExperimental = true; // Needed in core profile
#endif

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
	GLenum glew = glewInit();
	if(glew!= GLEW_OK)
		Log(Log::DIE) << "Failed to initialize GLEW";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif

	// Create a Vertex Buffer Object and copy the vertex data to it
	glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attributes), triangle_attributes, GL_STATIC_DRAW);

	GLuint vertexShader = loadShader("shaders/triangle.v.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader("shaders/triangle.f.glsl", GL_FRAGMENT_SHADER);

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);

	GLint link_ok = GL_FALSE;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
		Log(Log::DIE) << "Error in glLinkProgram";

    // Specify the layout of the vertex data
    attribute_coord3d = glGetAttribLocation(shaderProgram, "coord3d");
	if(attribute_coord3d == -1)
		Log(Log::DIE) << "Could not bind attribute coord3d";

	attribute_v_color = glGetAttribLocation(shaderProgram, "v_color");
	if(attribute_v_color == -1)
		Log(Log::DIE) << "Could not bind attribute v_color";

	uniform_fade = glGetUniformLocation(shaderProgram, "fade");
	if(uniform_fade == -1)
		Log(Log::DIE) << "Could not bind uniform attribute fade";

	Log() << "Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	Log() << "Quit";
}

GLuint Window::loadShader(const char * file, GLenum type)
{
	GLuint shader = glCreateShader(type);

	std::string lines = readFile(file);
	auto ptr = lines.c_str();

	const char* precision =
		"#ifdef GL_ES                        \n"
		"#  ifdef GL_FRAGMENT_PRECISION_HIGH \n"
		"     precision highp float;         \n"
		"#  else                             \n"
		"     precision mediump float;       \n"
		"#  endif                            \n"
		"#endif                              \n";

	const GLchar* sources[] = {
		precision,
		ptr
	}; // When adding update sizeof below

	glShaderSource(shader, 2, sources, NULL);
	glCompileShader(shader);

	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

	if(!compile_ok)
		Log(Log::DIE) << "Error in vertex shader\n" << getGlLog(shader);

	return shader;
}

std::string Window::readFile(const char* filename)
{
	std::ifstream t(filename);
	if(t.fail())
		Log(Log::DIE) << "Cannot open " << filename;

	return std::string((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());
}

std::string Window::getGlLog(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else
		return "getGlLog: Not a shader or a program";

	std::string ret;
	ret.resize(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, &ret[0]);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, &ret[0]);
	
	return ret;
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
	float cur_fade = sinf(SDL_GetTicks() / 1000.0 * (2*3.14) / 5) / 2 + 0.5;

	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glUniform1f(uniform_fade, cur_fade);

	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);

	glVertexAttribPointer(
		attribute_coord3d,   // attribute
		3,                   // number of elements per vertex, here (x,y,z)
		GL_FLOAT,            // the type of each element
		GL_FALSE,            // take our values as-is
		sizeof(struct attributes),  // next coord3d appears every 5 floats
		0                    // offset of first element
	);
	glVertexAttribPointer(
		attribute_v_color,      // attribute
		3,                      // number of elements per vertex, here (r,g,b)
		GL_FLOAT,               // the type of each element
		GL_FALSE,               // take our values as-is
		sizeof(struct attributes),  // stride
		//(void*) (2 * sizeof(GLfloat))     // offset of first element
		(void*) offsetof(struct attributes, v_color)  // offset
	);
	

	// Draw a triangle from the 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	SDL_GL_SwapWindow(mWindow);
}

