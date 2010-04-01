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

struct maze_markov_bellman_value
{
	enum maze_markov_action action;
	float cost;
};

struct maze_markov_bellman_node
{
	struct maze_markov_bellman_value *value;
	struct maze_markov_bellman_node *next;
};

struct maze_markov_bellman_list
{
	struct maze_markov_state *state;
	struct maze_markov_bellman_node *list;
};

struct maze_markov_bellman
{
	struct maze_markov_bellman_list *list;
};


struct maze_markov_bellman_vlist *maze_markov_bellman_vlist_create(struct maze_markov_state *s,float c,struct maze_markov_bellman_vlist *l);
void maze_markov_bellman_vlist_destroy(struct maze_markov_bellman_vlist *l);

#endif
