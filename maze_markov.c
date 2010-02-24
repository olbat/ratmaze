#include "maze_markov.h"
#include "maze.h"
#include <stdio.h>
#include <stdlib.h>


struct maze_markov_chain *maze_markov_chain_create(struct maze *m)
{
	struct maze_markov_chain *ret;

	ret = (struct maze_markov_chain *)
		malloc(sizeof(struct maze_markov_chain));

	ret->init = maze_markov_state_build(m,0,0,0,&(ret->goal),
		MAZE_MARKOV_STATE_PATH_CAME_FROM_NOWHERE);

	return ret;
}

void maze_markov_chain_destroy(struct maze_markov_chain *mc)
{
	maze_markov_state_destroy(mc->init,
		MAZE_MARKOV_STATE_PATH_CAME_FROM_NOWHERE);

	free(mc);
}

struct maze_markov_state *
maze_markov_state_create(unsigned int x, unsigned int y, struct square *s)
{
	struct maze_markov_state *ret;

	ret = (struct maze_markov_state *) 
		malloc(sizeof(struct maze_markov_state));

	ret->coord.x = x;
	ret->coord.y = y;
	ret->square = s;
	
	return ret;
}

void maze_markov_state_remove(struct maze_markov_state *s)
{
	free(s);
}

struct maze_markov_state *
maze_markov_state_build(
	struct maze *m,
	unsigned int x,
	unsigned int y,
	struct maze_markov_state *origins,
	struct maze_markov_state **goal,
	enum maze_markov_state_build_origin originp
)
{
	struct maze_markov_state *ret;

	if ((!MAZE_IS_VALID_POSITION(m,x,y)) || (!MAZE_GET_SQUARE(m,x,y)))
		ret = 0;
	else
	{
		ret = maze_markov_state_create(x,y,MAZE_GET_SQUARE(m,x,y));
		
		/*
		if (MAZE_IS_GOAL(m,x,y))
			*goal = ret;
		*/

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_UP)
			ret->up = origins;
		else
		{
			ret->up = maze_markov_state_build(m,x,y+1,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN)
			ret->down = origins;
		else
		{
			ret->down = maze_markov_state_build(m,x,y-1,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT)
			ret->left = origins;
		else
		{
			ret->left = maze_markov_state_build(m,x-1,y,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT)
			ret->right = origins;
		else
		{
			ret->right = maze_markov_state_build(m,x+1,y,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
		}
	}

	return ret;
}

void maze_markov_state_destroy(
	struct maze_markov_state *current,
	enum maze_markov_state_build_origin originp
)
{
	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_UP)
	{
		maze_markov_state_destroy(current->up,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN)
	{
		maze_markov_state_destroy(current->down,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT)
	{
		maze_markov_state_destroy(current->left,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT)
	{
		maze_markov_state_destroy(current->right,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
	}

	maze_markov_state_remove(current);
}

void maze_markov_chain_display(struct maze_markov_chain *mc)
{
	printf("Markov Chain:\n");
	maze_markov_state_display(mc->init,
		MAZE_MARKOV_STATE_PATH_CAME_FROM_NOWHERE);
	printf("---\n");
}

void maze_markov_state_display(
	struct maze_markov_state *cur,
	enum maze_markov_state_build_origin originp
)
{
	if (!cur)
		return;

	printf("(%d,%d) [u:%d,d:%d,l:%d,r:%d]\n", cur->coord.x, cur->coord.y,
		(cur->up ? 1 : 0),(cur->down ? 1 : 0),(cur->left ? 1 : 0),
		(cur->right ? 1 : 0));

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_UP)
	{
		maze_markov_state_display(cur->up,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN)
	{
		maze_markov_state_display(cur->down,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT)
	{
		maze_markov_state_display(cur->left,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
	}

	if (originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT)
	{
		maze_markov_state_display(cur->right,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
	}
}
