#include "maze_solver_policy_iteration.h"
#include "maze_markov.h"
#include "maze_markov_bellman.h"
#include "maze_solver_value_iteration.h"
#include "ratmaze.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <unistd.h>
#include <math.h>

struct maze_solver_pi_list *maze_solver_pi_list_create(
	unsigned int t,
	struct maze_solver_pi_list *prec,
	struct maze_markov_bellman_vlist *vlist,
	struct maze_markov_bellman_policy *p
)
{
	struct maze_solver_pi_list *ret;

	ret = (struct maze_solver_pi_list *)
		malloc(sizeof(struct maze_solver_pi_list));

	ret->t = t;
	ret->prec = prec;
	ret->vlist = vlist;
	ret->policy = p;

	return ret;
}

void maze_solver_pi_list_destroy(struct maze_solver_pi_list *l)
{
	struct maze_solver_pi_list *tmpl;

	while ((tmpl = l->prec))
	{
		maze_markov_bellman_vlist_destroy(l->vlist);
		maze_markov_bellman_policy_destroy(l->policy);
		free(l);
		l = tmpl;
	}
	maze_markov_bellman_vlist_destroy(l->vlist);
	maze_markov_bellman_policy_destroy(l->policy);
	free(l);
}

float maze_solver_pi_get_vlist_cost(
	struct maze_markov_state *state,
	struct maze_markov_bellman_vlist *prec,
	struct maze_markov_bellman_policy *policy
)
{
	float ret;
	struct maze_markov_transition_list *l;

	l = MAZE_MARKOV_GET_TRANSITION_LIST(
		state,
		maze_markov_bellman_policy_get_action(state,policy)
	);

	if (l)
		ret = maze_markov_bellman_get_action_cost(l,prec);
	else
		ret = 0.0f;

#ifdef MAZE_DEBUG
	printf("V(%d)=%g\n",state->id,ret);
#endif
	
	return ret;	
}

struct maze_markov_bellman_vlist *maze_solver_vi_pi_perform_results(
	struct maze_markov_decision_process *mdp,
	struct maze_markov_bellman_policy *policy
)
{
	struct maze_markov_bellman_vlist *ret;
	unsigned int t;
	float maxs, tmpf;
	struct maze_markov_bellman_vlist *node, *nodenew;
	struct maze_solver_vi_list *tlist, *prectlist;
	struct maze_markov_state_list *sl;
	struct maze_markov_bellman_list *qlist;

	ret = 0;

	t = 0;
	prectlist = 0;

	do
	{
		sl = mdp->states;
		node = 0;
		
		while (sl)
		{
			node = maze_markov_bellman_vlist_create(
				sl->node,
				maze_solver_pi_get_vlist_cost(
					sl->node,
					(prectlist?prectlist->vlist:0),
					policy
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

	/* find the optimal solution */
	ret = tlist->vlist;
	qlist = maze_markov_bellman_qlist_create(mdp,tlist->vlist);
	policy = maze_markov_bellman_optimal_policy_create(qlist);
	ret = maze_markov_bellman_policy_vlist_wrapper(policy);
	
	maze_solver_vi_list_destroy(tlist);
	maze_markov_bellman_list_destroy(qlist);
	maze_markov_bellman_policy_destroy(policy);

	return ret;
}

void maze_solver_pi_perform(struct maze_markov_decision_process *mdp)
{
	unsigned int t;
	struct maze_markov_bellman_vlist *node, *vstar;
	struct maze_solver_pi_list *tlist, *prectlist;
	struct maze_markov_state_list *sl;
	struct maze_markov_bellman_list *qlist;
	struct maze_markov_bellman_policy *policy, *precpolicy;
#ifdef MAZE_DEBUG
	char buffc;
#endif

	t = 0;
	prectlist = 0;

	sl = mdp->states;
	policy = precpolicy = 0;
	while (sl)
	{
		policy = maze_markov_bellman_policy_create(
			sl->node,	
			MAZE_SOLVER_PI_DEFAULT_POLICY_ACTION,
			MAZE_SOLVER_PI_DEFAULT_POLICY_COST,
			policy
		);
		sl = sl->next;
	}

	do
	{
#ifdef MAZE_DEBUG
		printf("[t:%d]\n",t);
#endif
		vstar = maze_solver_vi_pi_perform_results(mdp,policy);

		sl = mdp->states;
		node = 0;
		
		while (sl)
		{
			node = maze_markov_bellman_vlist_create(
				sl->node,
				maze_solver_pi_get_vlist_cost(
					sl->node,
					vstar,
					policy
				),
				node
			);
			sl = sl->next;
		}

#ifdef MAZE_DEBUG
		buffc = read(0,&buffc,sizeof(buffc));
#endif
		tlist = maze_solver_pi_list_create(t,prectlist,node,policy);
		
		precpolicy = policy;
		qlist = maze_markov_bellman_qlist_create(mdp,tlist->vlist);
		policy = maze_markov_bellman_optimal_policy_create(qlist);
		maze_markov_bellman_list_destroy(qlist);
		maze_markov_bellman_vlist_destroy(vstar);

		prectlist = tlist;
		t++;
	} while (!maze_markov_bellman_policy_compare(policy,precpolicy));
	printf("Policy Iteration: %d iterations\n",t);

	/* find the optimal solution */
	qlist = maze_markov_bellman_qlist_create(mdp,tlist->vlist);
	printf("\nQ List:\n");
	maze_markov_bellman_list_display(qlist);
	printf("\nOptimal policy:\n");
	maze_markov_bellman_policy_display(policy);
	printf("---\n");
	
	maze_markov_bellman_policy_destroy(policy);
	maze_solver_pi_list_destroy(tlist);
	maze_markov_bellman_list_destroy(qlist);
}
