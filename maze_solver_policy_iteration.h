#ifndef _MAZE_SOLVER_POLICY_ITERATION_H
#define _MAZE_SOLVER_POLICY_ITERATION_H

#include "maze_markov_bellman.h"

#define MAZE_SOLVER_PI_DEFAULT_POLICY_ACTION MAZE_MARKOV_ACTION_UP
#define MAZE_SOLVER_PI_DEFAULT_POLICY_COST 0.0f

struct maze_solver_pi_list
{
	unsigned int t;
	struct maze_markov_bellman_vlist *vlist;
	struct maze_markov_bellman_policy *policy;

	struct maze_solver_pi_list *prec;
};

struct maze_solver_pi_list *maze_solver_pi_list_create(unsigned int t,struct maze_solver_pi_list *prec,struct maze_markov_bellman_vlist *vlist,struct maze_markov_bellman_policy *p);
void maze_solver_pi_list_destroy(struct maze_solver_pi_list *l);

float maze_solver_pi_get_vlist_cost(struct maze_markov_state *state,struct maze_markov_bellman_vlist *prec,struct maze_markov_bellman_policy *policy);

void maze_solver_pi_perform(struct maze_markov_decision_process *mdp);

#endif
