#ifndef _MAZE_SOLVER_VALUE_ITERATION_H
#define _MAZE_SOLVER_VALUE_ITERATION_H

#include "maze_markov.h"


#define MAZE_SOLVER_VI_EPSYLON 0.01f
#define MAZE_SOLVER_VI_V0 0.0f

struct maze_solver_vi_value
{
	struct maze_markov_state *state;
	float cost;
};

struct maze_solver_vi_node
{
	struct maze_solver_vi_value *value;
	struct maze_solver_vi_node *next;
};

struct maze_solver_vi_list
{
	unsigned int t;
	struct maze_solver_vi_node *list;
};

#endif
