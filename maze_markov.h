#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "maze.h"

struct maze_markov_chain
{
	struct maze_markov_state *init;
	struct maze_markov_state *goal;
};

enum maze_markov_state_build_origin
{
	MAZE_MARKOV_STATE_PATH_CAME_FROM_NOWHERE,
	MAZE_MARKOV_STATE_PATH_CAME_FROM_UP,
	MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN,
	MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT,
	MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT
};

/* Each state transition has the same cost */
struct maze_markov_state
{
	struct point coord;
	struct square *square;
	
	struct maze_markov_state *up;
	struct maze_markov_state *down;
	struct maze_markov_state *left;
	struct maze_markov_state *right;
};


struct maze_markov_chain *maze_markov_chain_create(struct maze *m);
void maze_markov_chain_destroy(struct maze_markov_chain *mc);

struct maze_markov_state *maze_markov_state_create(unsigned int x, unsigned int y, struct square *s);
void maze_markov_state_remove(struct maze_markov_state *s);

struct maze_markov_state *maze_markov_state_build(struct maze *m,unsigned int x,unsigned int y,struct maze_markov_state *origins,struct maze_markov_state **goal,enum maze_markov_state_build_origin originp);

void maze_markov_state_destroy(struct maze_markov_state *current,enum maze_markov_state_build_origin originp);

void maze_markov_chain_display(struct maze_markov_chain *mc);
void maze_markov_state_display(struct maze_markov_state *ms,enum maze_markov_state_build_origin originp);

#endif
