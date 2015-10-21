#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <limits.h>
#include <stdlib.h>
#include "global.h"

//size used for the viewport
#define GAME_SIZE 10
#define INF 1000000

/* Debugging controls .*/
enum debugFlags {
	debug_time,
	debug_wall,
	debug_initialise_particle,
	debug_particle,
	debug_particle_collision,
	debug_collideParticlesBruteForce,
	debug_collideParticlesUniformGrid,
	debug_collisionReactionParticles2DbasisChange,
	debug_collisionReactionParticles2DprojNormal,
	debug_framerate,
	debug_range_check,
	debug_sum_kinetic_energy,
	debug_sum_momentum,
	numDebugFlags
};
extern int debug[numDebugFlags];

/* Use our type so we can change precision easily. */
typedef double Real;

/* Small number to handle numerical imprecision. */
const Real epsilon = 1.0e-6;

/* Control 1D or 2D */
const int dimension = 2;

/* Control random or explicit initial positions */
enum {
	randomly,
	explicitly
} const initialiseParticles = randomly;

/* Control collision reaction calculation */
extern enum ReactionCalculation {
	basisChange,
	projNormal
} reacCalc;

/* Particles (particles). */
struct Particle {
	Real position[2];
	Real velocity[2];
	float color[3];
	Real radius;
	Real mass;
	Real elasticity;
	int slices, loops;    /* For rendering. */
};


/* Arena. */
struct Arena {
	Real min[2], max[2];
	Real momentum[2];
};
extern struct Arena arena;

/* Rendering info. */
//extern enum renderMode { wire, solid };
enum renderMode { wire, solid };
extern renderMode renMode;
extern Real elapsedTime, startTime, diffTime;
const int milli = 1000;
extern bool go;

/* Collision detection method. */
enum CollisionDetectionMethod {
	bruteForce,
	uniformGrid
};

extern CollisionDetectionMethod CDmethod;

extern struct Particle ball;
extern struct Particle *pegs;
extern int numPegs;
#endif
