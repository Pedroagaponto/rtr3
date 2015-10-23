#include "global.h"
#include "renderer.h"
#include "input.h"
#include "collision.h"
#include <time.h>

void pegWave(vec2 *pegsInit, int nPegs, int nWaves, float yOffset);
void initialisePegs (void);
void initialiseBall (void);
void initialisePolygons (int *vertex[2], int nSides, int angleOffset, int radius);
void myInit (void);
	
int debug[numDebugFlags] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 };
struct Arena arena;
renderMode renMode = wire;
Real elapsedTime = 0.0, startTime = 0.0, diffTime = 0;
bool go = false;
CollisionDetectionMethod CDmethod = bruteForce;

struct Particle ball;
struct Particle *pegs;
int *collidedPegs;
int *movingParticles;
int numPegs = 0;
int levelPoints = 0;
bool movingPegs = false;
ReactionCalculation reacCalc = basisChange;

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(500, 500);
	glutCreateWindow("Collision Detection and Reaction: Particles in an Arena");
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(keyboardCB);

	myInit();

	glutMainLoop();
}

void pegWave(vec2 *pegsInit, int nPegs, int nWaves, float yOffset) 
{
	for (int i = 0; i < nPegs; i ++)
	{
		float aux = (float)i/(float)nPegs;
		pegsInit[i].x = -GAME_SIZE + (GAME_SIZE * aux * 2) + (GAME_SIZE/nPegs);
		pegsInit[i].y = sin(nWaves * 2 * M_PI * aux) + yOffset;
	}
}

void initialisePegs (void)
{
	int nSpecialPegs = RAND(0, 5);
	vec2 pegsInit[50];

	for (int i = 0; i < 5; i++) {
		pegWave(pegsInit+(10*i), 10, 3, (i*2) - 5);
	}

	numPegs = sizeof(pegsInit) / sizeof(pegsInit[0]);
	printf ("numPegs = %d\n", numPegs);
	pegs = (Particle *) malloc(sizeof(Particle) * numPegs);
	collidedPegs = (int *) calloc(numPegs, sizeof(int));
	movingParticles = (int *) calloc(numPegs, sizeof(int));

	for (int i = 0; i < numPegs; i++) {
		pegs[i].velocity[0] = 0.0;
		pegs[i].velocity[1] = 0.0;
		pegs[i].radius = 0.3;
		pegs[i].mass = 1.0;
		pegs[i].elasticity = 0.5;
		pegs[i].slices = 10;
		pegs[i].loops = 3;
		pegs[i].position[0] = pegsInit[i].x;
		pegs[i].position[1] = pegsInit[i].y;
		pegs[i].color[0] = 1;
		pegs[i].color[1] = 1;
		pegs[i].color[2] = 1;
		pegs[i].canMove = false;
	}

	for (int i = 0; i < nSpecialPegs; i++)
	{
		int index = RAND(0, numPegs);
		pegs[index].color[0] = 0;
		pegs[index].color[1] = 0;
		pegs[index].canMove = true;
	}

}

void initialiseBall (void)
{
	ball.canMove = true;
	ball.velocity[0] = 0.0;
	ball.velocity[1] = INITIAL_VELOCITY;
	ball.radius = 0.2;
	ball.mass = 0.5;
	ball.elasticity = 0.5;
	ball.slices = 10;
	ball.loops = 3;
	ball.position[0] = 0;
	ball.position[1] = GAME_SIZE - 1;
	ball.color[0] = 1;
	ball.color[1] = 1;
	ball.color[2] = 1;
}

void initialisePolygons (int *vertex[2], int nSides, int angleOffset, int radius)
{
	if (nSides < 3)
		return;

	float theta = 2 * M_PI / nSides;
	float c0 = cos(theta);
	float s0 = sin(theta);
	float tx;

	//Inicial position
	float x = cos(angleOffset) * radius;
	float y = sin(angleOffset) * radius;

	for(int i = 0; i < nSides; i++)
	{
		vertex[i][0] = x;
		vertex[i][1] = y;

		tx = x;
		x = c0 * x - s0 * y;
		y = s0 * tx + c0 * y;
	}

}

void myInit (void)
{
	setRenderMode(renMode);
	initialisePegs();
	initialiseBall();
}

