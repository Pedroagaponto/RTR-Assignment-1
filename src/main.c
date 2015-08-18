/*
 * Simple 3D sine wave animation example.
 * $Id: sinewave3D.c,v 1.3 2015/08/04 03:04:43 gl Exp gl $
 */

#include "core.h"
#include "input.h"
#include "draw.h"

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <SDL2/SDL.h>

void consolePM(void);
void mainLoop(void);
void displayMultiView(void);
void display(void);
void idle(void);

int main(int argc, char **argv)
{
	UNUSED_VAR argc;
	UNUSED_VAR argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return EXIT_SUCCESS;
	}

	if (!initGraphics()) {
		SDL_Quit();
		return EXIT_FAILURE;
	}

	init();
	atexit(sys_shutdown);
	mainLoop();

	return EXIT_SUCCESS;
}

// Console performance meter
void consolePM(void)
{
	printf("frame rate (f/s):  %5.3f\n", g.frameRate);
	printf("frame time (ms/f): %5.3f\n", 1.0 / g.frameRate * 1000.0);
	printf("tesselation:       %5d\n", g.tess);
}

/*
// On screen display 
void displayOSD()
{
	char buffer[30];
	char *bufp;
	int w, h;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//Set up orthographic coordinate system to match the window, 
	//   i.e. (0,0)-(w,h)
	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, w, h);

	// Frame rate 
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(10, 60);
	snprintf(buffer, sizeof buffer, "frame rate (f/s):  %5.0f", g.frameRate);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	// Frame time 
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(10, 40);
	snprintf(buffer, sizeof buffer, "frame time (ms/f): %5.0f", 1.0 / g.frameRate * milli);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	// Tesselation
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(10, 20);
	snprintf(buffer, sizeof buffer, "tesselation:       %5d", g.tess);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	glPopMatrix();  //Pop modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();  // Pop projection 
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}
*/


void mainLoop(void)
{
	while (1)
	{
		eventDispatcher();
		if (getWantRedisplay())
		{
			if (!getMultiViewDisplay())
				display();
			else
				displayMultiView();

			disableWantRedisplay();
		}
		idle();
	}
}

void displayMultiView(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// Front view
	glPushMatrix();
	glViewport(g.width / 16.0, g.height * 9.0 / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);
	drawAxes(5.0);
	drawSineWave(g.tess);
	glPopMatrix();

	// Top view
	glPushMatrix();
	glViewport(g.width / 16.0, g.height / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	drawAxes(5.0);
	drawSineWave(g.tess);
	glPopMatrix();

	// Left view
	glPushMatrix();
	glViewport(g.width * 9.0 / 16.0, g.height * 9.0 / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	drawAxes(5.0);
	drawSineWave(g.tess);
	glPopMatrix();

	// General view
	glPushMatrix();
	glViewport(g.width * 9.0 / 16.0, g.width / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);

	glRotatef(camera.rotateX, 1.0, 0.0, 0.0);
	glRotatef(camera.rotateY, 0.0, 1.0, 0.0);
	glScalef(camera.scale, camera.scale, camera.scale);
	drawAxes(5.0);
	drawSineWave(g.tess);
	glPopMatrix();

//	if (g.displayOSD)
//		displayOSD();

	if (g.consolePM)
		consolePM();

	SDL_GL_SwapWindow(getMainWindow());
	g.frameCount++;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glViewport(0, 0, g.width, g.height);

	// General view
	glPushMatrix();

	glRotatef(camera.rotateX, 1.0, 0.0, 0.0);
	glRotatef(camera.rotateY, 0.0, 1.0, 0.0);
	glScalef(camera.scale, camera.scale, camera.scale);
	drawAxes(5.0);
	drawSineWave(g.tess);
	glPopMatrix();

//	if (g.displayOSD)
//		displayOSD();

	if (g.consolePM)
		consolePM();

	SDL_GL_SwapWindow(getMainWindow());
	g.frameCount++;
}

void steadyfps (int fps)
{
	static int old_time = 0;
	int current_time, wait_time;

	current_time = SDL_GetTicks();

	wait_time = 1000.0/(float)fps -(current_time - old_time);

	if (wait_time > 0)
		SDL_Delay(wait_time);
	else
		old_time = current_time;

	old_time += 1000.0/(float)fps;

}

void idle(void)
{
	float t, dt;

	t = SDL_GetTicks() / getMilli();

	// Accumulate time if animation enabled
	if (g.animate) {
		dt = t - g.lastT; 
		g.t += dt;
		g.lastT = t;
		if (getDebug(d_animation))
			printf("idle: animate %f\n", g.t);
	}

	// Update stats, although could make conditional on a flag set interactively
	dt = (t - g.lastStatsDisplayT);
	if (dt > g.displayStatsInterval) {
		g.frameRate = g.frameCount / dt;
		if (getDebug(d_OSD))
			printf("dt %f framecount %d framerate %f\n", dt, g.frameCount, g.frameRate);
		g.lastStatsDisplayT = t;
		g.frameCount = 0;
	}

	postRedisplay();

	if (g.steadyFps)
		steadyfps(30);
}

