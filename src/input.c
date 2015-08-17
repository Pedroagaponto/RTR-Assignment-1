#include "input.h"
#include "core.h"

#include <stdio.h>
#include <stdbool.h>

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
				if (getDebug(d_reshape))
					printf("SDL_WINDOWEVENT_RESIZED\n");
				if (event.window.windowID == SDL_GetWindowID(getMainWindow())) {
					SDL_SetWindowSize(getMainWindow(),
							event.window.data1, event.window.data2);
					reshape(event.window.data1, event.window.data2);
					postRedisplay();
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
			mouse(event.button.button, event.button.x, event.button.y);
			break;

			case SDL_MOUSEBUTTONUP:
			camera.control = inactive;
			break;

			default:
			break;
		}
	}
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
				g.lastT = SDL_GetTicks() / getMilli();
			} 
			break;
		case SDLK_l:
			g.lighting = !g.lighting;
			postRedisplay();
			break;
		case SDLK_m:
			printf("%d\n", g.polygonMode);
			if (g.polygonMode == line)
				g.polygonMode = fill;
			else
				g.polygonMode = line;
			postRedisplay();
			break;
		case SDLK_n:
			g.drawNormals = !g.drawNormals;
			postRedisplay();
			break;
		case SDLK_c:
			g.consolePM = !g.consolePM;
			postRedisplay();
			break;
		case SDLK_o:
			g.displayOSD = !g.displayOSD;
			postRedisplay();
			break;
		case SDLK_s:
			switchMultiViewDisplay();
			postRedisplay();
			break;
		case SDLK_KP_PLUS:
		case SDLK_PLUS:
		case SDLK_EQUALS:
			g.tess *= 2;
			postRedisplay();
			break;
		case SDLK_KP_MINUS:
		case SDLK_MINUS:
			g.tess /= 2;
			if (g.tess < 8)
				g.tess = 8;
			postRedisplay();
			break;
		case SDLK_d:
			g.waveDim++;
			if (g.waveDim > 3)
				g.waveDim = 2;
			postRedisplay();
			break;
		case SDLK_f:
			g.steadyFps = !g.steadyFps;
			postRedisplay();
			break;
		default:
			break;
	}
}

void mouse(int button, int x, int y)
{
	if (getDebug(d_mouse))
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

	postRedisplay();
}

void motion(int x, int y)
{
	float dx, dy;

	if (getDebug(d_mouse)) {
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

	postRedisplay();
}

