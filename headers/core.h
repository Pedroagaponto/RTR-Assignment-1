#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define WIDTH 1024
#define HEIGHT 1024
#define NLIGHTS 9

#include <GL/gl.h>

#define UNUSED_VAR (void)

void setDebug(bool value, int position);
void postRedisplay(void);
void disableWantRedisplay(void);
void switchMultiViewDisplay(void);
void setGlContext(SDL_GLContext value);
void setMainWindow(SDL_Window *value);
bool getDebug(int position);
bool getWantRedisplay(void);
bool getMultiViewDisplay(void);
float getMilli(void);
SDL_GLContext getGlContext(void);
SDL_Window* getMainWindow(void);
void init(void);
void reshape(int w, int h);
bool initGraphics(void);

typedef struct vec3f {
	float x, y, z;
} vec3f;

typedef enum {
	d_reshape,
	d_drawSineWave,
	d_mouse,
	d_key,
	d_animation,
	d_lighting,
	d_OSD,
	d_nflags
} DebugFlags;

typedef struct {
	bool animate, lighting, drawNormals, displayOSD, consolePM, steadyFps;
	float t, lastT, frameRate, displayStatsInterval;
	enum { line, fill } polygonMode;
	int width, height, tess, waveDim, frameCount, lastStatsDisplayT, n_lights;
} Global;

typedef enum {
	inactive,
	rotate,
	pan,
	zoom
} CameraControl;

typedef struct {
	int lastX, lastY;
	float rotateX, rotateY, scale;
	CameraControl control;
} Camera;

extern Camera camera;
extern Global g;

#endif
