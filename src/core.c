#include "core.h"
#include <stdio.h>

Camera camera = {0, 0, 30.0, -30.0, 1.0, inactive};
Global g = {false, false, false, false, false, false,
			0.0, 0.0, 0.0, 1.0,
			line,
			0, 0, 8, 2, 0, 0, 1};

static bool debug[d_nflags] = {false, false, false, false, false, false, false};
static bool wantRedisplay = false;
static bool multiViewDisplay = false;
static const float milli = 1000.0;
static SDL_GLContext glContext;
static SDL_Window *mainWindow = 0;

GLfloat lightpos[][4] = {
	{ -1.0, -1.0, -1.0, 0.0 }, // 1
	{  1.0, -1.0, -1.0, 0.0 }, // 2
	{  1.0,  1.0, -1.0, 0.0 }, // 3
	{ -1.0,  1.0, -1.0, 0.0 }, // 4
	{ -1.0, -1.0,  1.0, 0.0 }, // 5
	{  1.0, -1.0,  1.0, 0.0 }, // 6
	{  1.0,  1.0,  1.0, 0.0 }, // 7
	{ -1.0,  1.0,  1.0, 0.0 }  // 8
};

GLfloat ambient[] = {0, 0, 0, 1},
		diffuse[] = {1, 1, 1, 1},
		specular[] = {1, 1, 1, 1};

void setDebug(bool value, int position)
{
	debug[position] = value;
}

void postRedisplay(void)
{
	wantRedisplay = true;
}

void disableWantRedisplay(void)
{
	wantRedisplay = false;
}

void switchMultiViewDisplay(void)
{
	multiViewDisplay = !multiViewDisplay;
}

void setGlContext(SDL_GLContext value)
{
	glContext = value;
}

void setMainWindow(SDL_Window *value)
{
	mainWindow = value;
}

bool getDebug(int position)
{
	return debug[position];
}

bool getWantRedisplay(void)
{
	return wantRedisplay;
}

bool getMultiViewDisplay(void)
{
	return multiViewDisplay;
}

float getMilli(void)
{
	return milli;
}

SDL_GLContext getGlContext(void)
{
	return glContext;
}

SDL_Window* getMainWindow(void)
{
	return mainWindow;
}

void init(void)
{
	int i;
	glClearColor(0.0, 0.0, 0.0, 1.0);

	for(i = 0; i < NLIGHTS-1; i++)
	{
		glLightfv(GL_LIGHT1 + i, GL_POSITION, lightpos[i]);
		glLightfv(GL_LIGHT1 + i, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1 + i, GL_SPECULAR, specular);
	}
	glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
	g.width = w;
	g.height = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool initGraphics(void)
{
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	mainWindow = SDL_CreateWindow("Assignment 1",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!mainWindow)
	{
		fprintf(stderr, "Failed to create a window: %s\n", SDL_GetError());
		return false;
	}

	glContext = SDL_GL_CreateContext(mainWindow);
	SDL_GL_MakeCurrent(mainWindow, glContext);
	reshape(WIDTH, HEIGHT);

	return true;
}

