#ifndef _MAZE_MARKOV_BELLMAN_H
#define _MAZE_MARKOV_BELLMAN_H

#include "maze_markov.h"


#define MAZE_MARKOV_BELLMAN_GAMMA 0.99f


struct maze_markov_bellman_vlist
{
	struct maze_markov_state *state;
	float cost;

	struct maze_markov_bellman_vlist *next;
};

struct maze_markov_bellman_node
{
	enum maze_markov_action action;
	float cost;

	struct maze_markov_bellman_node *next;
};

struct maze_markov_bellman_list
{
	struct maze_markov_state *state;
	struct maze_markov_bellman_node *list;

	struct maze_markov_bellman_list *next;
};

struct maze_markov_bellman_policy
{
	struct maze_markov_state *state;
	enum maze_markov_action action;
	float cost;

	struct maze_markov_bellman_policy *next;
};

struct maze_markov_bellman_vlist *maze_markov_bellman_vlist_create(struct maze_markov_state *s,float c,struct maze_markov_bellman_vlist *l);
void maze_markov_bellman_vlist_destroy(struct maze_markov_bellman_vlist *l);

__inline__ float maze_markov_bellman_get_action_cost(struct maze_markov_transition_list *l,struct maze_markov_bellman_vlist *prec);

struct maze_markov_bellman_node *maze_markov_bellman_node_create(struct maze_markov_state *s,enum maze_markov_action a,struct maze_markov_bellman_vlist *vstar,struct maze_markov_bellman_node *n);
struct maze_markov_bellman_node *maze_markov_bellman_node_default_create(struct maze_markov_state *s,enum maze_markov_action a,float cost,struct maze_markov_bellman_node *n);
void maze_markov_bellman_node_destroy(struct maze_markov_bellman_node *v);

struct maze_markov_bellman_list *maze_markov_bellman_list_create(struct maze_markov_state *s,struct maze_markov_bellman_node *l,struct maze_markov_bellman_list *n);
void maze_markov_bellman_list_destroy(struct maze_markov_bellman_list *l);


struct maze_markov_bellman_policy *maze_markov_bellman_policy_create(struct maze_markov_state *s,enum maze_markov_action a,float c,struct maze_markov_bellman_policy *n);
void maze_markov_bellman_policy_destroy(struct maze_markov_bellman_policy *l);

void maze_markov_bellman_qlist_set_cost(struct maze_markov_bellman_list *l,struct maze_markov_state *s,enum maze_markov_action a,float val);
float maze_markov_bellman_qlist_get_cost(struct maze_markov_bellman_list *l,struct maze_markov_state *s,enum maze_markov_action a);

struct maze_markov_bellman_list *maze_markov_bellman_default_qlist_create(struct maze_markov_decision_process *mdp, float defcost);
struct maze_markov_bellman_list *maze_markov_bellman_qlist_create(struct maze_markov_decision_process *mdp,struct maze_markov_bellman_vlist *vstar);

struct maze_markov_bellman_policy *maze_markov_bellman_optimal_policy_create(struct maze_markov_bellman_list *qlist);

void maze_markov_bellman_list_display(struct maze_markov_bellman_list *l);
void maze_markov_bellman_policy_display(struct maze_markov_bellman_policy *l);

#endif
