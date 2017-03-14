# Real-Time Rendering & 3D Games Programming - Assignment 1

This code was implemented as part of a RMIT University course and it's based on a code provided at the *[course website][ASSIGNMENT_LINK]*.

## Controls

| Keys | Action                                                                               |
| ---- | ------------------------------------------------------------------------------------ |
| ESC  | Exit program                                                                         |
| a    | Toggle wave animation                                                                |
| l    | Toggle lighting mechanism                                                            |
| m    | Switch between wireframe mode and filled mode                                        |
| n    | Switch normals renderization on/off                                                  |
| c    | Switch perfmeter print on console on/off                                             |
| s    | Switch between wave simple view and for way split screen                             |
| +    | Increases tesselation up to 1024                                                     |
| -    | Decreases tesselation down to 8                                                      |
| d    | Toggle wave dimention                                                                |
| f    | Toggle stedy FPS                                                                     |
| v    | One push switches to VBO mode. Two pushes switches to VBO mode with Single Strip VBO |
| i    | Switches to immediate mode                                                           |
| 0    | No lights enabled                                                                    |
| 1    | One light enabled                                                                    |
| 2    | Two lights enabled                                                                   |
| 3    | Three lights enabled                                                                 |
| 4    | Four lights enabled                                                                  |
| 5    | Five lights enabled                                                                  |
| 6    | Six lights enabled                                                                   |
| 7    | Seven lights enabled                                                                 |
| 8    | Eight lights enabled                                                                 |

## Extra Features

Dynamically allocates the arrays used and reallocates them whenever the tesselation changes. 

One of the modes (toggled by 'v') uses just one strip for rendering the wave.

One of the modes implements a steady frame rate of around 30 fps

## Build

Simple compilation with gcc compiler:

    $ make

## Dependencies

This project depends on `SDL2` and `OpenGL` libraries to compile.

Therefore, please make sure you have the packages *[SDL2][SDL_LINK]* and *[OpenGL][OPENGL_LINK]*.


[ASSIGNMENT_LINK]: http://goanna.cs.rmit.edu.au/~gl/teaching/rtr&3dgp/assignments/sinewave3D.c
[SDL_LINK]: https://wiki.libsdl.org/Installation
[OPENGL_LINK]: https://www.opengl.org/
