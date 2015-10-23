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
	UNUSED_VAR x;
	UNUSED_VAR y;

	switch (key) {
		case 'a':
		case 'A':
			if (!go) {
				theta = STEP_ROTATION;
				rotateBall(-theta * M_PI/180.0);
			}
			break;

		case 'd':
		case 'D':
			if (!go) {
				theta = STEP_ROTATION;
				rotateBall(theta * M_PI/180.0);
			}
			break;

		case 'z':
		case 'Z':
			if (!go) {
				theta = STEP_ROTATION/8.0;
				rotateBall(-theta * M_PI/180.0);
			}
			break;

		case 'c':
		case 'C':
			if (!go) {
				theta = STEP_ROTATION/8.0;
				rotateBall(theta * M_PI/180.0);
			}
			break;

		case 'q':
		case 'Q':
		case 27:
			exit(EXIT_SUCCESS);
			break;
		case 'w':
		case 'W':
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
		case 'R':
			if (reacCalc == projNormal)
				reacCalc = basisChange;
			else if (reacCalc == basisChange)
				reacCalc = projNormal;
			break;
		case 'm':
		case 'M':
			movingPegs = !movingPegs;
			break;
		case ' ':
			if (!go) {
				startTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli;
				go = true;
			}
			break;
	}
	glutPostRedisplay();
}

