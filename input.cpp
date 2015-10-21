#include "global.h"
#include "renderer.h"
#include "input.h"

#define STEP_ROTATION 5

void rotateBall(Real theta)
{
	Real tx, ty;
	float c0 = cos(theta);
	float s0 = sin(theta);

	//rotation matrix
	tx = ball.velocity[0];
	ty = ball.velocity[1];
	ball.velocity[0] = c0 * tx - s0 * ty;
	ball.velocity[1] = s0 * tx + c0 * ty;
}

void keyboardCB(unsigned char key, int x, int y)
{
	Real theta;

	switch (key) {
		case 'a':
			theta = STEP_ROTATION;
			rotateBall(-theta * M_PI/180.0);
			break;

		case 'd':
			theta = STEP_ROTATION;
			rotateBall(theta * M_PI/180.0);
			break;

		case 'z':
			theta = STEP_ROTATION/8.0;
			rotateBall(-theta * M_PI/180.0);
			break;

		case 'c':
			theta = STEP_ROTATION/8.0;
			rotateBall(theta * M_PI/180.0);
			break;

		case 'q':
			exit(EXIT_SUCCESS);
			break;
		case 'w':
			changeRenderMode();
			break;
/*		case 'c':
			debug[debug_range_check] = !debug[debug_range_check];
			break;
		case 'd':
			if (CDmethod == uniformGrid)
				CDmethod = bruteForce;
			else if (CDmethod == bruteForce)
				CDmethod = uniformGrid;
			break;*/
		case 'r':
			if (reacCalc == projNormal)
				reacCalc = basisChange;
			else if (reacCalc == basisChange)
				reacCalc = projNormal;
			break;
		case 's':
			if (!go) {
				startTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli;
				go = true;
			}
			break;
	}
	glutPostRedisplay();
}

