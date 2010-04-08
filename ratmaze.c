#include "maze.h"
#include "maze_markov.h"
#include "maze_solver_value_iteration.h"
#include "maze_solver_policy_iteration.h"
#include "maze_solver_qlearning.h"

#include <stdio.h>
#include <stdlib.h>


int
main(int argc, char **argv)
{
	struct maze *m;
	struct square s;
	struct element *e;
	struct maze_markov_decision_process *mdp;
	int tmp;

	if (argc < 2)
	{
		printf( "usage: %s <algorithm> [Q-learling limit]\n"
			"\t[0] Value Iteration\n"
			"\t[1] Policy Iteration\n"
			"\t[2] Q-learning\n",
			argv[0]
		);
		exit(EXIT_SUCCESS);
	}
	
	m = maze_create(5,3);
	e = (__typeof__(e)) malloc(sizeof(struct element));

	s.element = e;
	e->name = 'R';
	e->type = MAZE_ELEM_TYPE_INIT;

	maze_add(m,&s,0,0);
	s.element = 0;
	maze_add(m,&s,0,1);
	e->name = 'G';
	e->reward = 1;
	e->type = MAZE_ELEM_TYPE_GOAL;
	s.element = e;
	maze_add(m,&s,0,2);
	s.element = 0;
	maze_add(m,&s,1,2);
	e->name = 'F';
	e->reward = 10;
	e->type = MAZE_ELEM_TYPE_GOAL;
	s.element = e;
	maze_add(m,&s,2,0);
	s.element = 0;
	maze_add(m,&s,2,1);
	maze_add(m,&s,2,2);
	maze_add(m,&s,3,2);
	maze_add(m,&s,4,0);
	maze_add(m,&s,4,1);
	maze_add(m,&s,4,2);


	maze_display(m);
	printf("\n");
	/*
	MAZE_ELEMENT_MOVE_UP(m,e);
	MAZE_ELEMENT_MOVE_UP(m,e);
	MAZE_ELEMENT_MOVE_RIGHT(m,e);
	MAZE_ELEMENT_MOVE_LEFT(m,e);
	MAZE_ELEMENT_MOVE_LEFT(m,e);
	MAZE_ELEMENT_MOVE_LEFT(m,e);
	maze_display(m);
	*/

	mdp = maze_markov_decision_process_create(m);

	maze_markov_decision_process_display(mdp);

	if ((tmp = atoi(argv[1])) == 0)
		maze_solver_vi_perform(mdp);
	else if (tmp == 1)
		maze_solver_pi_perform(mdp);
	else
	{
		maze_solver_qlearning_perform(
			mdp,
			(argv[2] 
				? atoi(argv[2])
				: MAZE_SOLVER_QLEARNING_DEFAULT_ITER
			)
		);
	}

	free(e);
	maze_delete(m);
	maze_markov_decision_process_destroy(mdp);

	return 0;
}
