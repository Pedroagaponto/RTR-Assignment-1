/*
 * Simple 3D sine wave animation example.
 * $Id: sinewave3D.c,v 1.3 2015/08/04 03:04:43 gl Exp gl $
 */

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <SDL2/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define WIDTH 1024
#define HEIGHT 1024

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
	float r, g, b;
} color3f;

typedef struct vec3f {
	float x, y, z;
} vec3f;

typedef struct {
	bool animate, lighting, drawNormals, displayOSD, consolePM;
	float t, lastT, frameRate, displayStatsInterval;
	enum { line, fill } polygonMode;
	int width, height, tess, waveDim, frameCount, lastStatsDisplayT;
} Global;

typedef enum {
	inactive,
	rotate,
	pan,
	zoom
} CameraControl;

struct {
	int lastX, lastY;
	float rotateX, rotateY, scale;
	CameraControl control;
} camera = {0, 0, 30.0, -30.0, 1.0, inactive};

Global g = {false, false, false, false, false,
			0.0, 0.0, 0.0, 1.0,
			line,
			0, 0, 8, 2, 0, 0};

static bool debug[d_nflags] = {false, false, false, false, false, false, false};
static bool wantRedisplay = false;
static bool multiViewDisplay = false;

static color3f cyan =	{1.0, 0.0, 1.0};
static color3f white =	{1.0, 1.0, 1.0};
static color3f grey =	{0.8, 0.8, 0.8};
static color3f black =	{1.0, 1.0, 1.0};

const float milli = 1000.0;
static SDL_GLContext glcontext;
static SDL_Window *mainWindow = 0;

void init(void) 
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
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

void drawAxes(float length)
{

	glPushAttrib(GL_CURRENT_BIT);
	glBegin(GL_LINES);

	/* x axis */
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-length, 0.0, 0.0);
	glVertex3f(length, 0.0, 0.0);

	/* y axis */
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -length, 0.0);
	glVertex3f(0.0, length, 0.0);

	/* z axis */
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, -length);
	glVertex3f(0.0, 0.0, length);

	glEnd();
	glPopAttrib();
}

void drawVector(vec3f *r, vec3f *v, float s, bool normalize, color3f *c)
{
	glPushAttrib(GL_CURRENT_BIT);
	glColor3fv((GLfloat *)c);
	glBegin(GL_LINES);
	if (normalize) {
		float mag = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
		v->x /= mag;
		v->y /= mag;
		v->z /= mag;
	}
	glVertex3fv((GLfloat *)r);
	glVertex3f(r->x + s * v->x, r->y + s * v->y, r->z + s * v->z);
	glEnd();
	glPopAttrib();
}

// Console performance meter
void consolePM()
{
	printf("frame rate (f/s):  %5.0f\n", g.frameRate);
	printf("frame time (ms/f): %5.0f\n", 1.0 / g.frameRate * 1000.0);
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

void drawSineWave(int tess)
{
	const float A1 = 0.25, k1 = 2.0 * M_PI, w1 = 0.25;
	const float A2 = 0.25, k2 = 2.0 * M_PI, w2 = 0.25;
	float stepSize = 2.0 / tess;
	vec3f r, n;
	int i, j;
	float t = g.t;

	if (g.lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat *) &white);
		glMaterialf(GL_FRONT, GL_SHININESS, 20.0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	} else {
		glDisable(GL_LIGHTING);
	}

	if (g.polygonMode == line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	for (j = 0; j < tess; j++) {

		// Sine wave
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= tess; i++) {
			r.x = -1.0 + i * stepSize;
			r.z = -1.0 + j * stepSize;

			if (g.waveDim == 2) {
				r.y = A1 * sinf(k1 * r.x + w1 * t);
				if (g.lighting) {
					n.x = - A1 * k1 * cosf(k1 * r.x + w1 * t);
					n.y = 1.0;
					n.z = 0.0;
				}
			} else if (g.waveDim == 3) {
				r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
				if (g.lighting) {
					n.x = - A1 * k1 * cosf(k1 * r.x + w1 * t);
					n.y = 1.0;
					n.z = - A2 * k2 * cosf(k2 * r.z + w2 * t);
				}
			}

			if (g.lighting) 
				glNormal3fv((GLfloat *)&n);
			glVertex3fv((GLfloat *)&r);

			r.z += stepSize;

			if (g.waveDim == 3) {
				r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
				if (g.lighting) {
					n.z = - A2 * k2 * cosf(k2 * r.z + w2 * t);
				}
			}

			if (g.lighting) 
				glNormal3fv((GLfloat *)&n);
			glVertex3fv((GLfloat *)&r);
		}
		glEnd();
	}

	if (g.lighting) {
		glDisable(GL_LIGHTING);
	}

	// Normals
	if (g.drawNormals) {
		for (j = 0; j <= tess; j++) {
			for (i = 0; i <= tess; i++) {
				r.x = -1.0 + i * stepSize;
				r.z = -1.0 + j * stepSize;

				n.y = 1.0;
				n.x = - A1 * k1 * cosf(k1 * r.x + w1 * t);
				if (g.waveDim == 2) {
					r.y = A1 * sinf(k1 * r.x + w1 * t);
					n.z = 0.0;
				} else {
					r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
					n.z = - A2 * k2 * cosf(k2 * r.z + w2 * t);
				}

				drawVector(&r, &n, 0.05, true, &cyan);
			}
		}
	}
}


void idle()
{
	float t, dt;

	t = SDL_GetTicks() / milli;

	// Accumulate time if animation enabled
	if (g.animate) {
		dt = t - g.lastT; 
		g.t += dt;
		g.lastT = t;
		if (debug[d_animation])
			printf("idle: animate %f\n", g.t);
	}

	// Update stats, although could make conditional on a flag set interactively
	dt = (t - g.lastStatsDisplayT);
	if (dt > g.displayStatsInterval) {
		g.frameRate = g.frameCount / dt;
		if (debug[d_OSD])
			printf("dt %f framecount %d framerate %f\n", dt, g.frameCount, g.frameRate);
		g.lastStatsDisplayT = t;
		g.frameCount = 0;
	}

	wantRedisplay = true;
}

void displayMultiView()
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

	SDL_GL_SwapWindow(mainWindow);
	g.frameCount++;
}

void display()
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

	SDL_GL_SwapWindow(mainWindow);
	g.frameCount++;
}

void keyboard(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
		case SDLK_ESCAPE:
			printf("exit\n");
			exit(0);
			break;
		case SDLK_a:
			g.animate = !g.animate;
			if (g.animate) {
				g.lastT = SDL_GetTicks() / milli;
			} 
			break;
		case SDLK_l:
			g.lighting = !g.lighting;
			wantRedisplay = true;
			break;
		case SDLK_m:
			printf("%d\n", g.polygonMode);
			if (g.polygonMode == line)
				g.polygonMode = fill;
			else
				g.polygonMode = line;
			wantRedisplay = true;
			break;
		case SDLK_n:
			g.drawNormals = !g.drawNormals;
			wantRedisplay = true;
			break;
		case SDLK_c:
			g.consolePM = !g.consolePM;
			wantRedisplay = true;
			break;
		case SDLK_o:
			g.displayOSD = !g.displayOSD;
			wantRedisplay = true;
			break;
		case SDLK_s:
			multiViewDisplay = !multiViewDisplay;
			wantRedisplay = true;
			break;
		case SDLK_KP_PLUS:
		case SDLK_PLUS:
		case SDLK_EQUALS:
			g.tess *= 2;
			wantRedisplay = true;
			break;
		case SDLK_KP_MINUS:
		case SDLK_MINUS:
			g.tess /= 2;
			if (g.tess < 8)
				g.tess = 8;
			wantRedisplay = true;
			break;
		case SDLK_d:
			g.waveDim++;
			if (g.waveDim > 3)
				g.waveDim = 2;
			wantRedisplay = true;
			break;
		default:
			break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (debug[d_mouse])
		printf("mouse: %d %d %d\n", button, x, y);

	camera.lastX = x;
	camera.lastY = y;

	switch(button) {
		case SDL_BUTTON_LEFT:
			camera.control = rotate;
			break;
		case SDL_BUTTON_MIDDLE:
			camera.control = pan;
			break;
		case SDL_BUTTON_RIGHT:
			camera.control = zoom;
			break;
	}

	wantRedisplay = true;
}

void motion(int x, int y)
{
	float dx, dy;

	if (debug[d_mouse]) {
		printf("motion: %d %d\n", x, y);
		printf("camera.rotate: %f %f\n", camera.rotateX, camera.rotateY);
		printf("camera.scale:%f\n", camera.scale);
	}

	dx = x - camera.lastX;
	dy = y - camera.lastY;
	camera.lastX = x;
	camera.lastY = y;

	switch (camera.control) {
		case inactive:
			break;
		case rotate:
			camera.rotateX += dy;
			camera.rotateY += dx;
			break;
		case pan:
			break;
		case zoom:
			camera.scale += dy / 100.0;
			break;
	}

	wantRedisplay = true;
}

void eventDispatcher()
{
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: 
			exit(EXIT_SUCCESS);

			case SDL_KEYDOWN:
			keyboard(event.key);
			break;

			case SDL_WINDOWEVENT:
			switch(event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				if (debug[d_reshape])
					printf("SDL_WINDOWEVENT_RESIZED\n");
				if (event.window.windowID == SDL_GetWindowID(mainWindow)) {
					SDL_SetWindowSize(mainWindow,
							event.window.data1, event.window.data2);
					reshape(event.window.data1, event.window.data2);
					wantRedisplay = true;
				}		
				break;

				default:
				break;
			}
			break;

			case SDL_MOUSEMOTION:
			motion(event.motion.x, event.motion.y);
			break;

			case SDL_MOUSEBUTTONDOWN:
			mouse(event.button.button, event.button.state,
					event.button.x, event.button.y);
			break;

			case SDL_MOUSEBUTTONUP:
			camera.control = inactive;
			break;

			default:
			break;
		}
	}
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

	glcontext = SDL_GL_CreateContext(mainWindow);
	SDL_GL_MakeCurrent(mainWindow, glcontext);
	reshape(WIDTH, HEIGHT);

	return true;
}

void mainLoop(void)
{
	while (1)
	{
		eventDispatcher();
		if (wantRedisplay)
		{
			if (!multiViewDisplay)
				display();
			else
				displayMultiView();

			wantRedisplay = false;
		}
		idle();
	}
}

void sys_shutdown()
{
	SDL_Quit();
}

int main(int argc, char **argv)
{
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
