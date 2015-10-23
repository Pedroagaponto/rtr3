#include "global.h"
#include "collision.h"

#define GRAVITY 10

static Real oldTime = 0;
static int roundPoints = 0;

bool checkCollision (struct Particle p1, struct Particle p2)
{
	if (p2.collided)
		return false;
	//circle collision
	Real d0 = p1.position[0] - p2.position[0];
	Real d1 = p1.position[1] - p2.position[1];
	Real distance2 = d0 * d0 + d1 * d1;
	Real radius = p1.radius + p2.radius;


	//printf ("%f\n", distance2 - radius * radius);
	if (distance2 <= radius * radius)
		return true;

	return false;
}

void collisionReaction(struct Particle &p1, struct Particle &p2)
{
	Real n[2], n_mag;
	Real projnv1, projnv2;
	Real m1, m2, v1i, v2i, v1f, v2f;

	/* Normal vector n between centres. */
	n[0] = p2.position[0] - p1.position[0];
	n[1] = p2.position[1] - p1.position[1];

	/* Normalise n. */
	n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
	n[0] /= n_mag;
	n[1] /= n_mag;

	/* Vector projection/component/resolute of velocity in n direction. */
	projnv1 = n[0] * p1.velocity[0] + n[1] * p1.velocity[1];
	projnv2 = n[0] * p2.velocity[0] + n[1] * p2.velocity[1];

	/* Use 1D equations to calculate final velocities in n direction. */
	v1i = projnv1;
	v2i = projnv2;
	m1 = p1.mass;
	m2 = p2.mass;

	if(!p1.canMove)
	{
		v2f = -v2i * p2.elasticity;
		v1f = v1i;
	}
	else if (!p2.canMove)
	{
		v1f = -v1i * p1.elasticity;
		v2f = v2i;
	}
	else
	{
		v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0  * m2 / (m1 + m2) * v2i;
		v2f = 2.0  * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;
	}

	/* Vector addition to solve for final velocity. */
	p1.velocity[0] += (v1f - v1i) * n[0];
	p1.velocity[1] += (v1f - v1i) * n[1];
	p2.velocity[0] += (v2f - v2i) * n[0];
	p2.velocity[1] += (v2f - v2i) * n[1];
}

void moveBall(Real t)
{
	if (t < 0)
	{
		ball.velocity[1] += -GRAVITY * t;
		ball.position[0] += ball.velocity[0] * t;
		ball.position[1] += ball.velocity[1] * t;
	}
	else
	{
		ball.position[0] += ball.velocity[0] * t;
		ball.position[1] += ball.velocity[1] * t;
		ball.velocity[1] += -GRAVITY * t;
	}
}

void movePegs(Real t)
{
	for (int i = 0; i < numPegs; i++) {
		if (pegs[i].velocity[0] == 0 && pegs[i].velocity[1] == 0)
			continue;

		if (t < 0)
		{
			pegs[i].velocity[1] += -GRAVITY * t;
			pegs[i].position[0] += pegs[i].velocity[0] * t;
			pegs[i].position[1] += pegs[i].velocity[1] * t;
		}
		else
		{
			pegs[i].position[0] += pegs[i].velocity[0] * t;
			pegs[i].position[1] += pegs[i].velocity[1] * t;
			pegs[i].velocity[1] += -GRAVITY * t;
		}
	}
}

void setHitColor(struct Particle &p)
{
	if (!p.canMove) {
		p.color[0] = 1;
		p.color[1] = p.color[2] = 0;
	}
}

void accountCollidedPegs(void)
{
	for (int i = 0; i < numPegs; i++)
		if (collidedPegs[i]) {
			pegs[i].collided = true;
		}
	levelPoints += roundPoints;
	roundPoints = 0;
}

void updateBall(void)
{
	moveBall(diffTime);
	movePegs(diffTime);
	if (ball.position[0] < -GAME_SIZE || ball.position[0] > GAME_SIZE) { //check wall collision
		ball.velocity[0] = -ball.velocity[0];
		moveBall(diffTime);
	}

	for (int i = 0; i < numPegs; i++) {
		if (checkCollision(ball, pegs[i])) {
			if (!collidedPegs[i]) {
				roundPoints += 10;
				collidedPegs[i] = 1;
			}
			setHitColor(pegs[i]);
			moveBall(-diffTime);
			collisionReaction(ball, pegs[i]);
			moveBall(diffTime);
			movePegs(diffTime);
		}
	}
}

void updatePegs() {
	for (int i = 0; i < numPegs; i++) {
		if (movingPegs && pegs[i].velocity[0] == 0 && pegs[i].velocity[1] == 0) {
			pegs[i].position[0] = (pegs[i].position[0] > GAME_SIZE) ?
				pegs[i].radius - GAME_SIZE : pegs[i].position[0]+=0.001;
		}

		if (pegs[i].velocity[0] == 0 && pegs[i].velocity[1] == 0)
			continue;
		
		if (pegs[i].position[0] < -GAME_SIZE || pegs[i].position[0] > GAME_SIZE) { //check wall collision
			pegs[i].velocity[0] = -pegs[i].velocity[0];
			moveBall(diffTime);
		}
		
		for (int j = 0; j < numPegs; j++) {	
			if (i != j && checkCollision(pegs[i], pegs[j])) {
				if (!collidedPegs[j]) {
					collidedPegs[j] = 1;
					roundPoints += 10;
				}
				setHitColor(pegs[j]);
				collisionReaction(pegs[i], pegs[j]);
				movePegs(diffTime);
			}
		}
	}
}

void reset(void)
{
	accountCollidedPegs();
	go = false;
	ball.canMove = true;
	ball.position[0] = 0;
	ball.position[1] = GAME_SIZE - 1;
	ball.velocity[0] = 0.0;
	ball.velocity[1] = INITIAL_VELOCITY;
	oldTime = 0;
	for (int i = 0; i < numPegs; i++) {
		pegs[i].velocity[0] = 0;
		pegs[i].velocity[1] = 0;
	}
}

void update(void)
{
	updatePegs();
	if (!go) {
		glutPostRedisplay();
		return;
	}

	elapsedTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli - startTime;
	diffTime = elapsedTime - oldTime;
	oldTime = elapsedTime;
	updateBall();

	for (int i = 0; i < numPegs; i++) {
		if (collidedPegs[i] && pegs[i].canMove &&
				pegs[i].position[1] > -GAME_SIZE)
		{
			glutPostRedisplay();
			return;
		}
	}

	if (ball.position[1] < -GAME_SIZE)
		reset();
	glutPostRedisplay();
}

void panic(const char *m) {
	printf("%s", m);
	exit(1);
}

