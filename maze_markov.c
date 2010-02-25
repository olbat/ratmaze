#include "maze_markov.h"
#include "maze.h"
#include <stdio.h>
#include <stdlib.h>

#define MAZE_MARKOV_TRANSITION_DEFAULT_COST 1


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
		struct maze_markov_state *tmp;

		ret = maze_markov_state_create(x,y,MAZE_GET_SQUARE(m,x,y));
		
		if (MAZE_IS_GOAL(m,x,y))
			*goal = ret;

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_UP)
		{
			ret->up = maze_markov_transition_create(origins,
				MAZE_MARKOV_TRANSITION_DEFAULT_COST);
		}
		else
		{
			tmp = maze_markov_state_build(m,x,y+1,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
			if (tmp)
			{
				ret->up = maze_markov_transition_create(tmp,
					MAZE_MARKOV_TRANSITION_DEFAULT_COST);
			}
			else
				ret->up = 0;
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN)
		{
			ret->down = maze_markov_transition_create(origins,
				MAZE_MARKOV_TRANSITION_DEFAULT_COST);
		}
		else
		{
			tmp = maze_markov_state_build(m,x,y-1,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
			if (tmp)
			{
				ret->down = maze_markov_transition_create(tmp,
					MAZE_MARKOV_TRANSITION_DEFAULT_COST);
			}
			else
				ret->down = 0;
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT)
		{
			ret->left = maze_markov_transition_create(origins,
				MAZE_MARKOV_TRANSITION_DEFAULT_COST);
		}
		else
		{
			tmp = maze_markov_state_build(m,x-1,y,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
			if (tmp)
			{
				ret->left = maze_markov_transition_create(tmp,
					MAZE_MARKOV_TRANSITION_DEFAULT_COST);
			}
			else
				ret->left = 0;
		}

		if (originp == MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT)
		{
			ret->right = maze_markov_transition_create(origins,
				MAZE_MARKOV_TRANSITION_DEFAULT_COST);
		}
		else
		{
			tmp = maze_markov_state_build(m,x+1,y,ret,goal,
				MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
			if (tmp)
			{
				ret->right = maze_markov_transition_create(tmp,
					MAZE_MARKOV_TRANSITION_DEFAULT_COST);
			}
			else
				ret->right = 0;
		}
	}

	return ret;
}

void maze_markov_state_destroy(
	struct maze_markov_state *cur,
	enum maze_markov_state_build_origin originp
)
{
	if (!cur)
		return;

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_UP) && (cur->up))
	{
		maze_markov_state_destroy(cur->up->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
		maze_markov_transition_remove(cur->up);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN) && (cur->down))
	{
		maze_markov_state_destroy(cur->down->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
		maze_markov_transition_remove(cur->down);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT) && (cur->left))
	{
		maze_markov_state_destroy(cur->left->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
		maze_markov_transition_remove(cur->left);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT) && (cur->right))
	{
		maze_markov_state_destroy(cur->right->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
		maze_markov_transition_remove(cur->right);
	}

	maze_markov_state_remove(cur);
}

struct maze_markov_transition *
maze_markov_transition_create(struct maze_markov_state *dest, float cost)
{
	struct maze_markov_transition *ret;

	ret = (struct maze_markov_transition *)
		malloc(sizeof(struct maze_markov_transition));

	ret->dest = dest;
	ret->cost = cost;

	return ret;
}

void maze_markov_transition_remove(struct maze_markov_transition *t)
{
	free(t);
}

void maze_markov_chain_display(struct maze_markov_chain *mc)
{
	printf("Markov Chain:\n");
	printf("[init:(%d,%d), goal:(%d,%d)]\n",
		mc->init->coord.x,mc->init->coord.y,
		(mc->goal ? mc->goal->coord.x : -1),
		(mc->goal ? mc->goal->coord.y : -1));
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

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_UP) && (cur->up))
	{
		maze_markov_state_display(cur->up->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_DOWN) && (cur->down))
	{
		maze_markov_state_display(cur->down->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_UP);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT) && (cur->left))
	{
		maze_markov_state_display(cur->left->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT);
	}

	if ((originp != MAZE_MARKOV_STATE_PATH_CAME_FROM_RIGHT) && (cur->right))
	{
		maze_markov_state_display(cur->right->dest,
			MAZE_MARKOV_STATE_PATH_CAME_FROM_LEFT);
	}
}
