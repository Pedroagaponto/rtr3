#include "global.h"
#include "renderer.h"
#include "input.h"
#include "collision.h"

int debug[numDebugFlags] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 };
struct Arena arena;
renderMode renMode = wire;
Real elapsedTime = 0.0, startTime = 0.0, diffTime = 0;
bool go = false;
CollisionDetectionMethod CDmethod = bruteForce;

struct Particle ball;
struct Particle *pegs;
int numPegs = 0;
ReactionCalculation reacCalc = basisChange;

void initialisePegs (void)
{
	int pegsInit[][2] = {{0, 0}};

	numPegs = sizeof(pegsInit) / sizeof(pegsInit[0]);
	printf ("numPegs = %d\n", numPegs);
	pegs = (Particle *) malloc(sizeof(Particle) * numPegs);

	for (int i = 0; i < numPegs; i++) {
		pegs[i].velocity[0] = 0.0;
		pegs[i].velocity[1] = 0.0;
		pegs[i].radius = 1.0;
		pegs[i].mass = INF;
		pegs[i].elasticity = 1.0;
		pegs[i].slices = 10;
		pegs[i].loops = 3;
		pegs[i].position[0] = pegsInit[i][0];
		pegs[i].position[1] = pegsInit[i][1];
		pegs[i].color[0] = 1;
		pegs[i].color[1] = 1;
		pegs[i].color[2] = 1;
	}

}

void initialiseBall (void)
{

	ball.velocity[0] = 0.0;
	ball.velocity[1] = -5.0;
	ball.radius = 1.0;
	ball.mass = 1.0;
	ball.elasticity = 0.9;
	ball.slices = 10;
	ball.loops = 3;
	ball.position[0] = 0;
	ball.position[1] = GAME_SIZE - 1;
	ball.color[0] = 1;
	ball.color[1] = 1;
	ball.color[2] = 1;
}

void myInit (void)
{
	setRenderMode(renMode);
	//initialiseArena();
	//if (initialiseParticles == explicitly)
	initialisePegs();
	initialiseBall();
	//else if (initialiseParticles == randomly)
	//	initialiseParticlesRandomly();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(500, 500);
	glutCreateWindow("Collision Detection and Reaction: Particles in an Arena");
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(keyboardCB);

	myInit();

	glutMainLoop();
}

