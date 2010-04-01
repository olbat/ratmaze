#include "maze_markov_bellman.h"
#include "maze_markov.h"

#include "stdio.h"
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

#define MAZE_MARKOV_BELLMAN_VLIST_LOOKUP(L,S) __extension__ \
({ \
	__typeof__(L) LST, RET; \
	LST = (L); \
	while ((LST) && (LST->state != (S))) \
		LST = LST->next; \
	if (LST->state == (S)) \
		RET = LST; \
	else \
		RET = 0; \
	RET; \
})

#define MAZE_MARKOV_BELLMAN_VLIST_LOOKUP_ITER(L,S) \
	while ((L) && (L->state != (S))) \
		L = L->next;


__inline__ float maze_markov_bellman_get_action_cost(
	struct maze_markov_transition_list *l,
	struct maze_markov_bellman_vlist *prec
)
{
	struct maze_markov_bellman_vlist *n;
	float preccost;
	float ret;

	ret = 0.0f;

	do
	{
		n = prec;

		MAZE_MARKOV_BELLMAN_VLIST_LOOKUP_ITER(n,l->dest);
		
		if (n)
			preccost = n->cost;
		else
			preccost = 0.0f;

		ret += (l->probability * 
			(l->dest->reward + 
				(MAZE_MARKOV_BELLMAN_GAMMA * preccost) 
			)
		);

		l = l->next;
	} while (l);

	return ret;
}

struct maze_markov_bellman_node *maze_markov_bellman_node_create(
	struct maze_markov_state *s,
	enum maze_markov_action a,
	struct maze_markov_bellman_vlist *vstar,
	struct maze_markov_bellman_node *n
)
{
	struct maze_markov_bellman_node *ret;
	struct maze_markov_transition_list *tl;

	if (!(tl = MAZE_MARKOV_GET_TRANSITION_LIST(s,a)))
		goto err;

	ret = (struct maze_markov_bellman_node *)
		malloc(sizeof(struct maze_markov_bellman_node));

	ret->action = a;
	ret->cost = maze_markov_bellman_get_action_cost(tl,vstar);
	ret->next = n;

	goto out;
err:
	return 0;
out:
	return ret;
}

void maze_markov_bellman_node_destroy(struct maze_markov_bellman_node *l)
{
	struct maze_markov_bellman_node *tmpl;

	while ((tmpl = l->next))
	{
		free(l);
		l = tmpl;
	}
	free(l);
}

struct maze_markov_bellman_list *maze_markov_bellman_list_create(
	struct maze_markov_state *s,
	struct maze_markov_bellman_node *l,
	struct maze_markov_bellman_list *n
)
{
	struct maze_markov_bellman_list *ret;

	ret = (struct maze_markov_bellman_list *)
		malloc(sizeof(struct maze_markov_bellman_list));

	ret->state = s;
	ret->list = l;
	ret->next = n;

	return ret;
}

void maze_markov_bellman_list_destroy(struct maze_markov_bellman_list *l)
{
	__typeof__(l) tmp;

	while ((tmp = l->next))
	{
		maze_markov_bellman_node_destroy(l->list);
		free(l);
		l = tmp;
	}
	maze_markov_bellman_node_destroy(l->list);
	free(l);
}

struct maze_markov_bellman_list *maze_markov_bellman_qlist_create(
	struct maze_markov_decision_process *mdp,
	struct maze_markov_bellman_vlist *vstar
)
{
	struct maze_markov_bellman_list *ret;
	__typeof__(mdp->states) l;
	struct maze_markov_bellman_node *n,*tmpn;
	unsigned int i;
	enum maze_markov_action possibleact[] = {
		MAZE_MARKOV_ACTION_UP,
		MAZE_MARKOV_ACTION_DOWN,
		MAZE_MARKOV_ACTION_LEFT,
		MAZE_MARKOV_ACTION_RIGHT
	};
		

	l = mdp->states;
	ret = 0;
	do
	{
		n = 0;
		for (i = 0; i < MAZE_MARKOV_ACTION_NB; i++)
		{
			tmpn = maze_markov_bellman_node_create(
				l->node,
				possibleact[i],
				vstar,
				n
			);
			if (tmpn)
				n = tmpn;
		}

		ret = maze_markov_bellman_list_create(l->node,n,ret);
		
		l = l->next;
	} while (l);

	return ret;
}

void maze_markov_bellman_list_display(struct maze_markov_bellman_list *l)
{
	__typeof__(l->list) ln;

	do
	{
		printf("\tState %d:\n",l->state->id);
		ln = l->list;
		do
		{
			printf("\t\tQ(%d,%s)=%g\n",
				l->state->id,
				MAZE_MARKOV_ACTION_NAMES(ln->action),
				ln->cost
			); 
			ln = ln->next;
		} while (ln);

		l = l->next;
	} while (l);
}

