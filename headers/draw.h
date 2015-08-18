#ifndef DRAW_H
#define DRAW_H

#include "core.h"

typedef struct {
	float r, g, b;
} color3f;

void drawAxes(float length);
void drawVector(vec3f *r, vec3f *v, float s, bool normalize, color3f *c);
void updateSineWave(void);
void drawSineWave(void);
void freeSineWaveArrays(void);

#endif
