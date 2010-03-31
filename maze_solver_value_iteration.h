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
	struct maze_solver_vi_node *vlist;

	struct maze_solver_vi_list *prec;
};

struct maze_solver_vi_list *maze_solver_vi_list_create(unsigned int t,struct maze_solver_vi_list *prec,struct maze_solver_vi_node *vlist);
void maze_solver_vi_list_destroy(struct maze_solver_vi_list *l);

struct maze_solver_vi_node *maze_solver_vi_node_create(	struct maze_solver_vi_value *v,struct maze_solver_vi_node *n);
void maze_solver_vi_node_destroy(struct maze_solver_vi_node *n);

__inline__ float maze_solver_vi_get_action_cost(struct maze_markov_transition_list *l,struct maze_solver_vi_node *prec);
struct maze_solver_vi_value *maze_solver_vi_value_create(struct maze_markov_state *state,struct maze_solver_vi_node *prec);
void maze_solver_vi_value_remove(struct maze_solver_vi_value *v);

void maze_solver_vi_perform(struct maze_markov_decision_process *mdp);


#endif
