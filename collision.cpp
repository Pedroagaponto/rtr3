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

	if(m1 == INF)
	{
		v2f = -v2i * p2.elasticity;
		v1f = v1i;
	}
	else if (m2 == INF)
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

void setHitColor(struct Particle &p)
{
	p.color[0] = 1;
	p.color[1] = p.color[2] = 0;
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
	for (int i = 0; i < numPegs; i++) {
		if (ball.position[0] < -GAME_SIZE || ball.position[0] > GAME_SIZE) { //check wall collision
			ball.velocity[0] = -ball.velocity[0];
			moveBall(diffTime);
		} else if (checkCollision(ball, pegs[i])) {
			roundPoints += 10;
			collidedPegs[i] = 1;
			setHitColor(pegs[i]);
			moveBall(-diffTime);
			collisionReaction(ball, pegs[i]);
			moveBall(diffTime);
		} else if (ball.position[1] < -GAME_SIZE) { //check end of round
			accountCollidedPegs();
			go = false;
			ball.position[0] = 0;
			ball.position[1] = GAME_SIZE - 1;
			ball.velocity[0] = 0.0;
			ball.velocity[1] = INITIAL_VELOCITY;
			oldTime = 0;
		}
	}
}

void update(void)
{
	if (!go)
		return;

	elapsedTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli - startTime;
	diffTime = elapsedTime - oldTime;
	oldTime = elapsedTime;
	updateBall();
	//updatePegs();

	glutPostRedisplay();
}

void panic(const char *m) {
	printf("%s", m);
	exit(1);
}

#if 0
void collideParticleWall(Particle &p, Arena &a)
{
	float dp[2];

	dp[0] = dp[1] = 0.0;
	if ((p.position[0] - p.radius) < a.min[0]) {
		p.position[0] +=
			2.0 * (a.min[0] - (p.position[0] - p.radius));
		p.velocity[0] *= -1.0;
		dp[0] += p.mass * -2.0 * p.velocity[0];
	}
	if ((p.position[1] - p.radius) < a.min[1]) {
		p.position[1] +=
			2.0 * (a.min[1] - (p.position[1] - p.radius));
		p.velocity[1] *= -1.0;
		dp[1] += p.mass * -2.0 * p.velocity[1];
	}
	if ((p.position[0] + p.radius) > a.max[0]) {
		p.position[0] -=
			2.0 * (p.position[0] + p.radius - a.max[0]);
		p.velocity[0] *= -1.0;
		dp[0] += p.mass * -2.0 * p.velocity[0];
	}
	if ((p.position[1] + p.radius) > a.max[1]) {
		p.position[1] -=
			2.0 * (p.position[1] + p.radius - a.max[1]);
		p.velocity[1] *= -1.0;
		dp[1] += p.mass * -2.0 * p.velocity[1];
	}
	arena.momentum[0] += dp[0];
	arena.momentum[1] += dp[1];
}

void collideParticlesWall(void)
{
	for (int i = 0; i < numParticles; i++) {
		if (debug[debug_wall])
			printf("%d %f %f\n",
					i, particle[i].position[0], particle[i].position[1]);
		collideParticleWall(particle[i], arena);
	}
}

void eulerStepSingleParticle(Particle &p, Real h)
{
	p.position[0] += h * p.velocity[0];
	p.position[1] += h * p.velocity[1];
}

void integrateMotionParticles(Real h)
{
	for (int i = 0; i < numParticles; i++)
		eulerStepSingleParticle(particle[i], h);
}

inline bool collisionDetectionParticles(Particle &p1, Particle &p2)
{
	Real sum_radii, sum_radii_sq, n[2], n_mag_sq;

	sum_radii = p1.radius + p2.radius;
	sum_radii_sq = sum_radii * sum_radii;
	n[0] = p2.position[0] - p1.position[0];
	n[1] = p2.position[1] - p1.position[1];
	n_mag_sq = n[0] * n[0] + n[1] * n[1];
	if (n_mag_sq <= sum_radii_sq)
		return true;
	else
		return false;
}

void collisionReactionParticles1D(Particle &p1, Particle &p2)
{
	float m1, m2, v1i, v1f, v2i, v2f;

	m1 = p1.mass;
	m2 = p2.mass;
	v1i = p1.velocity[0];
	v2i = p2.velocity[0];
	v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
	v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

	p1.velocity[0] = v1f;
	p2.velocity[0] = v2f;
}

void collisionReactionParticles2DprojNormal(Particle &p1, Particle &p2)
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
	v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
	v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

	/* Vector addition to solve for final velocity. */
	p1.velocity[0] = (p1.velocity[0] - v1i * n[0]) + v1f * n[0];
	p1.velocity[1] = (p1.velocity[1] - v1i * n[1]) + v1f * n[1];
	p2.velocity[0] = (p2.velocity[0] - v2i * n[0]) + v2f * n[0];
	p2.velocity[1] = (p2.velocity[1] - v2i * n[1]) + v2f * n[1];
}

void collisionReactionParticles2DbasisChange(Particle &p1, Particle &p2)
{
	Real n[2], t[2], n_mag;
	Real v1_nt[2], v2_nt[2];
	Real m1, m2, v1i, v2i, v1f, v2f;

	if (debug[debug_collisionReactionParticles2DbasisChange]) {
		printf("collisionReactionParticles2DbasisChange:\n");
		printf("velocities before: %f %f %f %f\n",
				p1.velocity[0], p1.velocity[1],
				p2.velocity[0], p2.velocity[1]);
	}

	/* Normal vector n between centres. */
	n[0] = p2.position[0] - p1.position[0];
	n[1] = p2.position[1] - p1.position[1];

	/* Normalise n. */
	n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
	n[0] /= n_mag;
	n[1] /= n_mag;

	/* Tangent vector t. */
	t[0] = -n[1];
	t[1] = n[0];

	/* Change basis for velocities from standard basis to nt basis. */
	v1_nt[0] = n[0] * p1.velocity[0] + n[1] * p1.velocity[1];
	v1_nt[1] = t[0] * p1.velocity[0] + t[1] * p1.velocity[1];
	v2_nt[0] = n[0] * p2.velocity[0] + n[1] * p2.velocity[1];
	v2_nt[1] = t[0] * p2.velocity[0] + t[1] * p2.velocity[1];

	/* Use 1D equations to calculate final velocities in n direction. */
	m1 = p1.mass;
	m2 = p2.mass;
	v1i = v1_nt[0];
	v2i = v2_nt[0];
	v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
	v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

	/* Update the 2D velocity. Force is in n direction, so in nt basis,
	 * velocity change is in n direction only, no change in t direction.
	 */
	v1_nt[0] = v1f;
	v2_nt[0] = v2f;

	/* Change back to standard basis. */
	p1.velocity[0] = n[0] * v1_nt[0] + t[0] * v1_nt[1];
	p1.velocity[1] = n[1] * v1_nt[0] + t[1] * v1_nt[1];
	p2.velocity[0] = n[0] * v2_nt[0] + t[0] * v2_nt[1];
	p2.velocity[1] = n[1] * v2_nt[0] + t[1] * v2_nt[1];

	if (debug[debug_collisionReactionParticles2DbasisChange]) {
		printf("velocities after: %f %f %f %f\n",
				p1.velocity[0], p1.velocity[1],
				p2.velocity[0], p2.velocity[1]);
	}
}

void collideParticlesBruteForce(Real h)
{
	int i, j;

	for (i = 0; i < numParticles - 1; i++) {
		for (j = i + 1; j < numParticles; j++) {
			if (collisionDetectionParticles(particle[i], particle[j])) {

				if (debug[debug_collideParticlesBruteForce])
					printf("collideParticlesBruteForce: collision %d %d\n", i, j);

				/* Take step back. Better approaches possible. */
				eulerStepSingleParticle(particle[i], -h);
				eulerStepSingleParticle(particle[j], -h);

				if (debug[debug_collideParticlesBruteForce]) {
					printf("velocities before: %f %f %f %f\n",
							particle[i].velocity[0], particle[i].velocity[1],
							particle[j].velocity[0], particle[j].velocity[1]);
				}

				/* Collision */
				if (dimension == 1)
					collisionReactionParticles1D(particle[i], particle[j]);
				else if (dimension == 2) {
					if (reacCalc == basisChange)
						collisionReactionParticles2DbasisChange(particle[i], particle[j]);
					else if (reacCalc == projNormal)
						collisionReactionParticles2DprojNormal(particle[i], particle[j]);
					else
						panic("collision reaction calculation not specified\n");
				}

				if (debug[debug_collideParticlesBruteForce]) {
					printf("velocities after: %f %f %f %f\n",
							particle[i].velocity[0], particle[i].velocity[1],
							particle[j].velocity[0], particle[j].velocity[1]);
				}

				/* Step forward. */
				eulerStepSingleParticle(particle[i], h);
				eulerStepSingleParticle(particle[j], h);

				if (debug[debug_collideParticlesBruteForce]) {
					printf("velocities after: %f %f %f %f\n",
							particle[i].velocity[0], particle[i].velocity[1],
							particle[j].velocity[0], particle[j].velocity[1]);
				}

				/* Check walls. */
				collideParticleWall(particle[i], arena);
				collideParticleWall(particle[j], arena);

				if (debug[debug_collideParticlesBruteForce]) {
					printf("velocities after: %f %f %f %f\n",
							particle[i].velocity[0], particle[i].velocity[1],
							particle[j].velocity[0], particle[j].velocity[1]);
				}
			}
		}
	}
}

inline void calcGridIndex(Particle &p, Arena a,
		Real gridCellSize[2], int gridNumCells[2],
		int index[2])
{
	index[0] = (int)((p.position[0] - a.min[0]) / gridCellSize[0]);
	index[1] = (int)((p.position[1] - a.min[1]) / gridCellSize[1]);
	if (debug_range_check) {
		if (index[0] < 0 || index[0] > gridNumCells[0] - 1)
			panic("gridIndex: index out of range\n");
		if (index[1] < 0 || index[1] > gridNumCells[1] - 1)
			panic("gridIndex: index out of range\n");
	}
}

void collideParticlesUniformGrid(Real h)
{
	Real gridCellSize[2];
	int **gridCellParticleCount, **gridCellParticleListEnd, *gridCellParticleList;
	int gridNumCells[2], gridSize, gridIndex[2], gridCellParticleListStart;
	int gridIndexMin[2], gridIndexMax[2];
	int i, j, k, s, t, p1, p2, total;

	/* Work out grid dimensions and allocate. */
	gridNumCells[0] = (int)(sqrt(numParticles) + 1);
	gridNumCells[1] = (int)(sqrt(numParticles) + 1);
	gridCellSize[0] = (arena.max[0] - arena.min[0]) / gridNumCells[0];
	gridCellSize[1] = (arena.max[1] - arena.min[1]) / gridNumCells[1];
	gridSize = gridNumCells[0] * gridNumCells[1];

	/* Assumption. */
	for (i = 0; i < numParticles; i++)
		if (particle[i].radius * 2.0 > gridCellSize[0] ||
				particle[i].radius * 2.0 > gridCellSize[1])
			panic("collideParticlesUniformGrid: particle diameter > cellSize\n");

	/* Allocate arrays. */
	gridCellParticleCount = (int **)malloc(gridNumCells[0] * sizeof(int *));
	if (gridCellParticleCount == 0)
		panic("collideParticlesUniformGrid: malloc failed\n");
	gridCellParticleListEnd = (int **)malloc(gridNumCells[0] * sizeof(int *));
	if (gridCellParticleListEnd == 0)
		panic("collideParticlesUniformGrid: malloc failed\n");
	for (i = 0; i < gridNumCells[0]; i++) {
		gridCellParticleCount[i] = (int *)malloc(gridNumCells[1] * sizeof(int));
		if (gridCellParticleCount[i] == 0)
			panic("collideParticlesUniformGrid: malloc failed\n");
		gridCellParticleListEnd[i] = (int *)malloc(gridNumCells[1] * sizeof(int));
		if (gridCellParticleListEnd[i] == 0)
			panic("collideParticlesUniformGrid: malloc failed\n");
	}
	gridCellParticleList = (int *)malloc(numParticles * sizeof(int));

	/* Initialise grid particle count. */
	for (i = 0; i < gridNumCells[0]; i++)
		for (j = 0; j < gridNumCells[1]; j++)
			gridCellParticleCount[i][j] = 0;

	/* Cell counts. */
	for (i = 0; i < numParticles; i++) {
		calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);
		gridCellParticleCount[gridIndex[0]][gridIndex[1]] += 1;
	}

	if (debug[debug_collideParticlesUniformGrid]) {
		printf("collideParticlesUniformGrid: gridCellParticleCount\n");
		for (i = 0; i < gridNumCells[0]; i++)
			for (j = 0; j < gridNumCells[1]; j++)
				printf("%d %d %d\n", i, j, gridCellParticleCount[i][j]);
	}

	/* Work out end of cell lists by accumulating cell counts. */
	for (i = 0; i < gridNumCells[0]; i++)
		for (j = 0; j < gridNumCells[1]; j++)
			gridCellParticleListEnd[i][j] = 0;

	total = 0;
	for (i = 0; i < gridNumCells[0]; i++)
		for (j = 0; j < gridNumCells[1]; j++) {
			total = total + gridCellParticleCount[i][j];
			gridCellParticleListEnd[i][j] = total - 1;
		}

	if (debug[debug_collideParticlesUniformGrid]) {
		printf("collideParticlesUniformGrid: gridCellParticleListEnd\n");
		for (i = 0; i < gridNumCells[0]; i++)
			for (j = 0; j < gridNumCells[1]; j++)
				printf("%d %d %d\n", i, j, gridCellParticleListEnd[i][j]);
	}

	/* Build particle lists. */
	for (i = 0; i < gridNumCells[0]; i++)
		for (j = 0; j < gridNumCells[1]; j++)
			gridCellParticleCount[i][j] = 0;

	for (i = 0; i < numParticles; i++) {
		calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);
		gridCellParticleList[gridCellParticleListEnd[gridIndex[0]][gridIndex[1]] -
			gridCellParticleCount[gridIndex[0]][gridIndex[1]]] = i;
		gridCellParticleCount[gridIndex[0]][gridIndex[1]] += 1;
	}

	if (debug[debug_collideParticlesUniformGrid]) {
		printf("collideParticlesUniformGrid: gridCellParticleList\n");
		for (i = 0; i < gridNumCells[0]; i++) {
			for (j = 0; j < gridNumCells[1]; j++) {
				gridCellParticleListStart =
					gridCellParticleListEnd[i][j] - gridCellParticleCount[i][j] + 1;
				printf("particle list %d %d\n", i, j);
				for (k = gridCellParticleListStart;
						k < gridCellParticleListEnd[i][j];
						k++)
					printf("%d\n", gridCellParticleList[k]);
				printf("\n");
			}
		}
	}

	/* Collision detection. */
	for (i = 0; i < numParticles; i++) {
		calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);

		/* Grid index bounds for this particle. */
		gridIndexMin[0] = gridIndex[0] - 1;
		if (gridIndexMin[0] < 0)
			gridIndexMin[0] = 0;
		gridIndexMin[1] = gridIndex[1] - 1;
		if (gridIndexMin[1] < 0)
			gridIndexMin[1] = 0;
		gridIndexMax[0] = gridIndex[0] + 1;
		if (gridIndexMax[0] > gridNumCells[0] - 1)
			gridIndexMax[0] = gridNumCells[0] - 1;
		gridIndexMax[1] = gridIndex[1] + 1;
		if (gridIndexMax[1] > gridNumCells[1] - 1)
			gridIndexMax[1] = gridNumCells[1] - 1;

		p1 = i;

		for (s = gridIndexMin[0]; s <= gridIndexMax[0]; s++) {
			for (t = gridIndexMin[1]; t <= gridIndexMax[1]; t++) {
				gridCellParticleListStart =
					gridCellParticleListEnd[s][t] - gridCellParticleCount[s][t] + 1;
				for (j = gridCellParticleListStart;
						j <= gridCellParticleListEnd[s][t];
						j++) {
					p2 = gridCellParticleList[j];

					/* Don't test particle against itself. */
					if (p2 == p1)
						continue;

					/* Only test pairs once. */
					if (p2 < p1)
						continue;

					if (debug[debug_collideParticlesUniformGrid])
						printf("collideParticlesUniformGrid: testing %d %d\n", p1, p2);

					if (collisionDetectionParticles(particle[p1], particle[p2])) {

						if (debug[debug_collideParticlesUniformGrid])
							printf("collision: %d %d\n", p1, p2);

						/* Take step back. Better approaches possible. */
						eulerStepSingleParticle(particle[p1], -h);
						eulerStepSingleParticle(particle[p2], -h);

						if (debug[debug_collideParticlesUniformGrid]) {
							printf("velocities before: %f %f %f %f\n",
									particle[p1].velocity[0], particle[p1].velocity[1],
									particle[p2].velocity[0], particle[p2].velocity[1]);
						}

						/* Collision */
						if (dimension == 1)
							collisionReactionParticles1D(particle[p1], particle[p2]);
						else if (dimension == 2) {
							if (reacCalc == basisChange)
								collisionReactionParticles2DbasisChange(particle[p1], particle[p2]);
							else if (reacCalc == projNormal)
								collisionReactionParticles2DprojNormal(particle[p1], particle[p2]);
							else
								panic("collision reaction calculation not specified\n");
						}

						if (debug[debug_collideParticlesUniformGrid])
							printf("velocities after: %f %f %f %f\n",
									particle[p1].velocity[0], particle[p1].velocity[1],
									particle[p1].velocity[0], particle[p2].velocity[1]);

						/* Step forward. */
						eulerStepSingleParticle(particle[p1], h);
						eulerStepSingleParticle(particle[p2], h);

						/* Check walls. */
						collideParticleWall(particle[p1], arena);
						collideParticleWall(particle[p2], arena);
					}
				}
			}
		}
	}

	/* Free arrays. */
	for (i = 0; i < gridNumCells[0]; i++) {
		free(gridCellParticleCount[i]);
		free(gridCellParticleListEnd[i]);
	}
	free(gridCellParticleCount);
	free(gridCellParticleListEnd);
	free(gridCellParticleList);
}

void updateParticles(void)
{
	static Real time = 0.0, h;

	/* Calculate time increment. */
	h = elapsedTime - time;
	time = elapsedTime;
	if (debug[debug_time])
		printf("updateParticles: time %f %f\n", time, h);

	/* Compute new positions of particles. */
	integrateMotionParticles(h);

	/* Collisions against walls. */
	collideParticlesWall();

	/* Collisions amongst particles. */
	if (CDmethod == bruteForce)
		collideParticlesBruteForce(h);
	else if (CDmethod == uniformGrid)
		collideParticlesUniformGrid(h);
	else
		panic("updateParticles: unknown collision detection method\n");

	/*if (debug[debug_sum_kinetic_energy]) {
		printf("K = %f\n", sumKineticEnergy());
	}
	if (debug[debug_sum_momentum]) {
		Real p[2];
		sumMomentum(p);
		printf("p = %f %f\n", p[0], p[1]);
	}*/
}
#endif
