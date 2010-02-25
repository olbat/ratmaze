#include "maze.h"
#include "maze_markov.h"
#include <stdio.h>
#include <stdlib.h>


int
main(void)
{
	struct maze *m;
	struct square s;
	struct element *e = malloc(sizeof(struct element));
	struct maze_markov_chain *mc;

	m = maze_create(5,3);
	s.element = e;
	e->name = 'R';
	e->type = MAZE_ELEM_TYPE_INIT;

	maze_add(m,&s,0,0);
	s.element = 0;
	maze_add(m,&s,0,1);
	maze_add(m,&s,0,2);
	maze_add(m,&s,1,2);
	e->name = 'F';
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

	mc = maze_markov_chain_create(m);

	maze_markov_chain_display(mc);

	free(e);
	maze_delete(m);
	maze_markov_chain_destroy(mc);

	return 0;
}
