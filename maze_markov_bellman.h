#ifndef _MAZE_MARKOV_BELLMAN_H
#define _MAZE_MARKOV_BELLMAN_H

#define MAZE_MARKOV_BELLMAN_GAMMA 0.99f

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
	unsigned int t;
	struct maze_markov_state *state;
	struct maze_markov_bellman_node *list;
};

struct maze_markov_bellman_value *maze_markov_bellman_value_create(unsigned int t,struct maze_markov_state *s,enum maze_markov_action action,struct maze_markov_bellman_list *prec);
void maze_markov_bellman_value_destroy(struct maze_markov_bellman_value *v);

#endif
