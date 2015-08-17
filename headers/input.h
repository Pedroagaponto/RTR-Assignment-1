#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

void eventDispatcher();
void keyboard(SDL_KeyboardEvent key);
void mouse(int button, int x, int y);
void motion(int x, int y);

#endif
