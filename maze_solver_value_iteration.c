#include "maze_solver_value_iteration.h"
#include "maze_markov.h"
#include "maze_markov_bellman.h"
#include "maze_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>


struct maze_solver_vi_list *maze_solver_vi_list_create(
	unsigned int t,
	struct maze_solver_vi_list *prec,
	struct maze_solver_vi_node *vlist
)
{
	struct maze_solver_vi_list *ret;

	ret = (struct maze_solver_vi_list *)
		malloc(sizeof(struct maze_solver_vi_list));

	ret->t = t;
	ret->prec = prec;
	ret->vlist = vlist;

	return ret;
}

void maze_solver_vi_list_destroy(struct maze_solver_vi_list *l)
{
	struct maze_solver_vi_list *tmpl;

	while ((tmpl = l->prec))
	{
		maze_solver_vi_node_destroy(l->vlist);
		free(l);
		l = tmpl;
	}
	maze_solver_vi_node_destroy(l->vlist);
	free(l);
}


struct maze_solver_vi_node *maze_solver_vi_node_create(
	struct maze_solver_vi_value *v,
	struct maze_solver_vi_node *n
)
{
	struct maze_solver_vi_node *ret;

	ret = (struct maze_solver_vi_node *) 
		malloc(sizeof(struct maze_solver_vi_node));

	ret->next = n;
	ret->value = v;

	return ret;
}

void maze_solver_vi_node_destroy(struct maze_solver_vi_node *n)
{
	struct maze_solver_vi_node *tmpn;

	while ((tmpn = n->next))
	{
		maze_solver_vi_value_remove(n->value);
		free(n);
		n = tmpn;
	}
	maze_solver_vi_value_remove(n->value);
	free(n);
}

/* #define MAZE_SOLVER_VI_GET_ACTION_COST() \ */
__inline__ float maze_solver_vi_get_action_cost(
	struct maze_markov_transition_list *l,
	struct maze_solver_vi_node *prec
)
{
	struct maze_solver_vi_node *n;
	float preccost;
	float ret;

	ret = 0.0f;

	do
	{
		n = prec;
		while ((n) && (n->value->state != l->dest))
			n = n->next;
		
		if (n)
			preccost = n->value->cost;
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

struct maze_solver_vi_value *maze_solver_vi_value_create(
	struct maze_markov_state *state,
	struct maze_solver_vi_node *prec
)
{
	struct maze_solver_vi_value *ret;
	float vup,vdown,vleft,vright, max;

	ret = (struct maze_solver_vi_value *) 
		malloc(sizeof(struct maze_solver_vi_value));

	vup = vdown = vleft = vright = max = FLT_MIN;

	ret->state = state;

	if (state->up)
	{
		vup = maze_solver_vi_get_action_cost(state->up,prec);
		max = vup;
	}
	if (state->down)
	{
		vdown = maze_solver_vi_get_action_cost(state->down,prec);
		max = MAX(vdown,max);	
	}
	if (state->left)
	{
		vleft = maze_solver_vi_get_action_cost(state->left,prec);
		max = MAX(vleft,max);	
	}
	if (state->right)
	{
		vright = maze_solver_vi_get_action_cost(state->right,prec);
		max = MAX(vright,max);	
	}

	ret->cost = max; 
	
	return ret;	
}

void maze_solver_vi_value_remove(struct maze_solver_vi_value *v)
{
	free(v);
}


void maze_solver_vi_list_display(struct maze_solver_vi_list *l)
{
}

void maze_solver_vi_perform(struct maze_markov_decision_process *mdp)
{
	unsigned int t;
	float maxs, tmpf;
	struct maze_solver_vi_node *node, *nodenew;
	struct maze_solver_vi_list *tlist, *prectlist;
	struct maze_markov_state_list *sl;


	t = 0;
	prectlist = 0;

	do
	{
		sl = mdp->states;
		node = 0;

		while (sl)
		{
			node = maze_solver_vi_node_create(
				maze_solver_vi_value_create(
					sl->node, 
					(prectlist?prectlist->vlist:0)
				),
				node
			);
			sl = sl->next;
		}
		tlist = maze_solver_vi_list_create(t,prectlist,node);
		
		/* perform max Vt(S) - Vt-1(S) */
		maxs = FLT_MIN;
		nodenew = tlist->vlist;
		while (nodenew)
		{
			node = (prectlist?prectlist->vlist:0);
			while (
				(node) && 
				(node->value->state != nodenew->value->state)
			)
				node = node->next;

			if (node)
				tmpf = (fabs(nodenew->value->cost - node->value->cost));
			else
				tmpf = (fabs(nodenew->value->cost - MAZE_SOLVER_VI_V0));
			if (tmpf > maxs)
				maxs = tmpf; 

			nodenew = nodenew->next;
		}
		
		prectlist = tlist;
		t++;
		printf("%g\n",maxs);
	} while (maxs > MAZE_SOLVER_VI_EPSYLON);
	maze_solver_vi_list_destroy(tlist);
	printf("Value Iteration: %d iterations, V%d: %g\n",t,t,maxs);
}
