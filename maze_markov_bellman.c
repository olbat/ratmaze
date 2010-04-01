#include "maze_markov_bellman.h"
#include "maze_markov.h"

#include "stdlib.h"

struct maze_markov_bellman_vlist *maze_markov_bellman_vlist_create(
	struct maze_markov_state *s,
	float c,
	struct maze_markov_bellman_vlist *n
)
{
	struct maze_markov_bellman_vlist *ret;

	ret = (struct maze_markov_bellman_vlist *) 
		malloc(sizeof(struct maze_markov_bellman_vlist));

	ret->state = s;
	ret->cost = c;
	ret->next = n;

	return ret;
}

void maze_markov_bellman_vlist_destroy(struct maze_markov_bellman_vlist *l)
{
	struct maze_markov_bellman_vlist *tmp;

	while ((tmp = l->next))
	{
		free(l);
		l = tmp;
	}
	free(l);
}

/*
struct maze_markov_bellman_value *maze_markov_bellman_value_create(
	unsigned int t,
	struct maze_markov_state *s,
	enum maze_markov_action action,
	struct maze_markov_bellman_list *prec
)
{
}

void maze_markov_bellman_value_destroy(struct maze_markov_bellman_value *v)
{
}
*/
