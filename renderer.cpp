#include "global.h"
#include "renderer.h"


extern renderMode renMode;

#define CIRCLE_PRECISION 10
void changeRenderMode(void)
{
	if (renMode == wire) {
		renMode = solid;
	} else {
		renMode = wire;
	}
	setRenderMode(renMode);
}

void setRenderMode(renderMode rm)
{
	if (rm == wire)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_NORMALIZE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (rm == solid)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
		glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void displayArena(void)
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(arena.min[0], arena.min[1], 0.0);
	glVertex3f(arena.max[0], arena.min[1], 0.0);
	glVertex3f(arena.max[0], arena.max[1], 0.0);
	glVertex3f(arena.min[0], arena.max[1], 0.0);
	glEnd();
}

void drawCircle(float r)
{
	float theta = 2 * M_PI / CIRCLE_PRECISION;
	float c0 = cos(theta);
	float s0 = sin(theta);
	float tx;

	//Inicial position (0 degree)
	float x = r;
	float y = 0;

	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < CIRCLE_PRECISION; i++)
	{
		glVertex2f(x, y);

		//rotation matrix
		tx = x;
		x = c0 * x  - s0 * y;
		y = s0 * tx + c0 * y;
	}
	glEnd();
}

void displayPegs(void)
{
	int i;

	/* Display particles. */
	for (i = 0; i < numPegs; i++) {
		if (debug[debug_particle])
			printf ("displayParticles: x %f y %f\n",
					pegs[i].position[0], pegs[i].position[1]);
		glPushMatrix();
		glTranslatef(pegs[i].position[0], pegs[i].position[1], 0.0);
		glColor3fv(pegs[i].color);
		drawCircle(pegs[i].radius);
		glPopMatrix();
	}
}

void displayBall(void)
{
	if (debug[debug_particle])
		printf ("displayParticles: x %f y %f\n",
				ball.position[0], ball.position[1]);
	glPushMatrix();
	glTranslatef(ball.position[0], ball.position[1], 0.0);
	glColor3fv(ball.color);
	drawCircle(ball.radius);

	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(ball.velocity[0], ball.velocity[1]);
	glEnd();

	glPopMatrix();
}

void displayOSD(int frameNo)
{
	static const Real interval = 1.0;
	static Real frameRateInterval = 0.0;
	static int frameNoStartInterval = 0;
	static Real elapsedTimeStartInterval = 0.0;
	static char buffer[80];
	int len, i;

	if (elapsedTime < interval)
		return;

	if (elapsedTime > elapsedTimeStartInterval + interval) {
		frameRateInterval = (frameNo - frameNoStartInterval) /
			(elapsedTime - elapsedTimeStartInterval);
		elapsedTimeStartInterval = elapsedTime;
		frameNoStartInterval = frameNo;
	}

	if (debug[debug_framerate]) {
		printf("displayOSD: frameNo %d elapsedTime %f "
				"frameRateInterval %f\n",
				frameNo, elapsedTime, frameRateInterval);
	}

	sprintf(buffer, "framerate: %5d frametime: %5d",
			int(frameRateInterval),
			int(1.0/frameRateInterval*1000));
	glRasterPos2f(-10,-9);
	len = (int)strlen(buffer);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
}

void drawAxis(void)
{
	glBegin(GL_LINES);
	for(int i = 0; i < 3; i++)
	{
		glColor3f(i == 0, i == 1, i == 2);
		glVertex3f(0, 0, 0);
		glVertex3f(i == 0, i == 1, i == 2);

	}
	glEnd();
}


void display(void)
{
	static int frameNo = 0;
	GLenum err;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawAxis();
	glColor3f (0.8, 0.8, 0.8);

	glPushMatrix();

	/* Display particle and arena. */
	glPushMatrix();
	//displayArena();
	displayPegs();
	displayBall();
	glPopMatrix();

	/*a Display frame rate counter. */
	glPushMatrix();
	displayOSD(frameNo);
	glPopMatrix();
	frameNo++;

	glPopMatrix();

	glutSwapBuffers();
	/* Check for errors. */
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("%s\n",gluErrorString(err));
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-GAME_SIZE, GAME_SIZE, -GAME_SIZE, GAME_SIZE, -GAME_SIZE, GAME_SIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

