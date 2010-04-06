#include "maze_solver_qlearning.h"
#include "maze_markov.h"
#include "maze_markov_bellman.h"
#include "ratmaze.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

void maze_solver_qlearning_perform(
	struct maze_markov_decision_process *mdp,
	unsigned int limit
)
{
	struct maze_markov_state *curs, *nexts;
	struct maze_markov_bellman_list *qlist, *tmplist;
	struct maze_markov_bellman_policy *policy;
	struct maze_markov_bellman_node *tmpnode;
	enum maze_markov_action cura;
	float alpha, reward, maxq;
	unsigned int i;

	curs = mdp->init;	
	alpha = MAZE_SOLVER_QLEARNING_DEFAULT_ALPHA;
	qlist = maze_markov_bellman_default_qlist_create(
		mdp,MAZE_SOLVER_QLEARNING_DEFAULT_Q0
	);
	i=0;

	srand(time(0));

	while (i < limit)
	{
		alpha = MAZE_SOLVER_QLEARNING_DEFAULT_ALPHA * ((limit - i) / (float)limit);
	
		cura = maze_markov_get_action_random();
		nexts = maze_markov_state_action_perform(
			mdp->states,curs,cura,&reward
		);
		
		/* get max(a')(Q(s',a')) */
		tmplist = qlist;
		while ((tmplist) && (tmplist->state != nexts))
			tmplist = tmplist->next;

		if (tmplist)
		{
			tmpnode = tmplist->list;
			maxq = FLT_MIN;
			while (tmpnode)
			{
				if (tmpnode->cost > maxq)
					maxq = tmpnode->cost;
				tmpnode = tmpnode->next;
			}
		}
		else
		{
			printf("\nERROR\n");
			maxq = MAZE_SOLVER_QLEARNING_DEFAULT_Q0;
		}

		maze_markov_bellman_qlist_set_cost(qlist,curs,cura,
			((1.0f - alpha)
				* maze_markov_bellman_qlist_get_cost(
					qlist,curs,cura
				)
			)
			+ (alpha * (reward + MAZE_MARKOV_BELLMAN_GAMMA * maxq))

		);
#ifdef MAZE_DEBUG
		printf("Step %d:\n\ts:%d, s':%d, a:%s\n\tQ(s,a)=%g\n\talpha=%g\n",
			i,curs->id,nexts->id,MAZE_MARKOV_ACTION_NAMES(cura),
			maze_markov_bellman_qlist_get_cost(qlist,curs,cura),
			alpha
		);
			
#endif
		curs = nexts;
		i++;
	}

	policy = maze_markov_bellman_optimal_policy_create(qlist);

	printf("\nQ List:\n");
	maze_markov_bellman_list_display(qlist);
	printf("\nOptimal policy:\n");
	maze_markov_bellman_policy_display(policy);
	printf("---\n");

	maze_markov_bellman_list_destroy(qlist);
	maze_markov_bellman_policy_destroy(policy);
}
