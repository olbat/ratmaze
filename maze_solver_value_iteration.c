#include "maze_solver_value_iteration.h"
#include "maze_markov.h"
#include "maze_markov_bellman.h"
#include "maze_math.h"
#include "ratmaze.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#ifdef MAZE_DEBUG
	#include <unistd.h>
#endif


struct maze_solver_vi_list *maze_solver_vi_list_create(
	unsigned int t,
	struct maze_solver_vi_list *prec,
	struct maze_markov_bellman_vlist *vlist
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
		maze_markov_bellman_vlist_destroy(l->vlist);
		free(l);
		l = tmpl;
	}
	maze_markov_bellman_vlist_destroy(l->vlist);
	free(l);
}


/*
__inline__ float maze_solver_vi_get_action_cost(
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
		while ((n) && (n->state != l->dest))
			n = n->next;
		
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
*/

float maze_solver_vi_get_vlist_cost(
	struct maze_markov_state *state,
	struct maze_markov_bellman_vlist *prec
)
{
	float ret;
	float vup,vdown,vleft,vright, max;

	vup = vdown = vleft = vright = max = FLT_MIN;

	if (state->up)
	{
		vup = maze_markov_bellman_get_action_cost(state->up,prec);
		max = vup;
	}
	if (state->down)
	{
		vdown = maze_markov_bellman_get_action_cost(state->down,prec);
		max = MAX(vdown,max);	
	}
	if (state->left)
	{
		vleft = maze_markov_bellman_get_action_cost(state->left,prec);
		max = MAX(vleft,max);	
	}
	if (state->right)
	{
		vright = maze_markov_bellman_get_action_cost(state->right,prec);
		max = MAX(vright,max);	
	}

	ret = max; 
#ifdef MAZE_DEBUG
	printf("V(%d)=%g\n",state->id,max);
#endif
	
	return ret;	
}

void maze_solver_vi_list_display(struct maze_solver_vi_list *l)
{
}

void maze_solver_vi_perform(struct maze_markov_decision_process *mdp)
{
	unsigned int t;
	float maxs, tmpf;
	struct maze_markov_bellman_vlist *node, *nodenew;
	struct maze_solver_vi_list *tlist, *prectlist;
	struct maze_markov_state_list *sl;
	struct maze_markov_bellman_list *qlist;
	struct maze_markov_bellman_policy *policy;
#ifdef MAZE_DEBUG
	char buffc;
#endif

	t = 0;
	prectlist = 0;

	do
	{
		sl = mdp->states;
		node = 0;
		
#ifdef MAZE_DEBUG
		printf("[t:%d]\n",t);
#endif
		while (sl)
		{
			node = maze_markov_bellman_vlist_create(
				sl->node,
				maze_solver_vi_get_vlist_cost(
					sl->node,
					(prectlist?prectlist->vlist:0)
				),
				node
			);
			sl = sl->next;
		}
#ifdef MAZE_DEBUG
		buffc = read(0,&buffc,sizeof(buffc));
#endif
		tlist = maze_solver_vi_list_create(t,prectlist,node);
		
		/* perform max Vt(S) - Vt-1(S) */
		maxs = FLT_MIN;
		nodenew = tlist->vlist;
		while (nodenew)
		{
			node = (prectlist?prectlist->vlist:0);
			while (
				(node) && 
				(node->state != nodenew->state)
			)
				node = node->next;

			if (node)
				tmpf = (fabs(nodenew->cost - node->cost));
			else
				tmpf = (fabs(nodenew->cost - MAZE_SOLVER_VI_V0));
			if (tmpf > maxs)
				maxs = tmpf; 

			nodenew = nodenew->next;
		}
		
		prectlist = tlist;
		t++;
	} while (maxs > MAZE_SOLVER_VI_EPSYLON);
	printf("Value Iteration: %d iterations, V%d: %g\n",t,t,maxs);

	/* find the optimal solution */
	qlist = maze_markov_bellman_qlist_create(mdp,tlist->vlist);
	policy = maze_markov_bellman_optimal_policy_create(qlist);
	printf("\nQ List:\n");
	maze_markov_bellman_list_display(qlist);
	printf("\nOptimal policy:\n");
	maze_markov_bellman_policy_display(policy);
	printf("---\n");
	
	maze_solver_vi_list_destroy(tlist);
	maze_markov_bellman_list_destroy(qlist);
	maze_markov_bellman_policy_destroy(policy);
}

