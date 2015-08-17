#include "draw.h"
#include <GL/gl.h>

static color3f cyan =	{1.0, 0.0, 1.0};
static color3f white =	{1.0, 1.0, 1.0};

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

