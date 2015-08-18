#ifndef DRAW_H
#define DRAW_H

#include "core.h"

#define NUM_BUFFERS 3

typedef struct {
	float r, g, b;
} color3f;

void drawAxes(float length);
void drawVector(vec3f *r, vec3f *v, float s, bool normalize, color3f *c);
void updateSineWave(void);
void drawSineWave(void);
void drawNormals(void);
void drawAsImmediate(void);
void freeSineWaveArrays(void);
void initVBO(void);
void unBindBuffers();
void bufferData(void);
void drawAsVBO(void);

#endif
