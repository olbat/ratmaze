#ifndef _MAZE_SOLVER_VALUE_ITERATION_H
#define _MAZE_SOLVER_VALUE_ITERATION_H

#include "maze_markov.h"


#define MAZE_SOLVER_VI_EPSYLON 0.01f
#define MAZE_SOLVER_VI_V0 0.0f

struct maze_solver_vi_list
{
	unsigned int t;
	struct maze_markov_bellman_vlist *vlist;

	struct maze_solver_vi_list *prec;
};

struct maze_solver_vi_list *maze_solver_vi_list_create(unsigned int t,struct maze_solver_vi_list *prec,struct maze_markov_bellman_vlist *vlist);
void maze_solver_vi_list_destroy(struct maze_solver_vi_list *l);

__inline__ float maze_solver_vi_get_action_cost(struct maze_markov_transition_list *l,struct maze_markov_bellman_vlist *prec);
float maze_solver_vi_get_vlist_cost(struct maze_markov_state *state,struct maze_markov_bellman_vlist *prec);

void maze_solver_vi_perform(struct maze_markov_decision_process *mdp);


#endif
