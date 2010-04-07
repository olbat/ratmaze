#include "maze_markov.h"
#include "maze_probabilities.h"
#include "maze.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAZE_MARKOV_TRANSITION_DEFAULT_COST 1


struct maze_markov_decision_process *maze_markov_decision_process_create(struct maze *m)
{
	struct maze_markov_decision_process *ret;

	ret = (struct maze_markov_decision_process *)
		malloc(sizeof(struct maze_markov_decision_process));

	ret->states = 0;

	ret->init = maze_markov_state_build(m,0,0,&ret->states);

	return ret;
}

void maze_markov_decision_process_destroy(struct maze_markov_decision_process *mdp)
{
	maze_markov_state_list_destroy(mdp->states);
	free(mdp);
}

struct maze_markov_state *
maze_markov_state_create(unsigned int x, unsigned int y, float reward)
{
	static unsigned int id = 0;
	struct maze_markov_state *ret;

	ret = (struct maze_markov_state *) 
		malloc(sizeof(struct maze_markov_state));

	ret->id = id++;
	ret->coord.x = x;
	ret->coord.y = y;
	ret->reward = reward;
	
	ret->up = 0;
	ret->down = 0;
	ret->left = 0;
	ret->right = 0;

	return ret;
}

void maze_markov_state_remove(struct maze_markov_state *s)
{
	maze_markov_transition_destroy(s->up);
	maze_markov_transition_destroy(s->down);
	maze_markov_transition_destroy(s->left);
	maze_markov_transition_destroy(s->right);
	free(s);
}

struct maze_markov_state *maze_markov_state_action_perform(
	struct maze_markov_state_list *l,
	struct maze_markov_state *s,
	enum maze_markov_action a,
	float *reward
)
{
	struct maze_markov_state *ret;

	ret = 0;
	while ((l) && (l->node != s))
		l = l->next;

	if (l)
	{
		struct maze_markov_transition_list *tl, *tmptl;
		float randf, randsum;

		tl = MAZE_MARKOV_GET_TRANSITION_LIST(s,a);

		randf = (float) (rand() / ((float)RAND_MAX + 1));

		randsum = 0.0f;

		while ((tmptl = tl->next) && (randsum < randf))
		{
			randsum += tl->probability;
			if (randsum < randf)
				tl = tmptl;
		}

		*reward = tl->dest->reward;
		ret = tl->dest;
	}

	return ret;
}

struct maze_markov_state_list *maze_markov_state_list_add(
	struct maze_markov_state_list *n,
	struct maze_markov_state *s
)
{
	struct maze_markov_state_list *ret;

	ret = (struct maze_markov_state_list *)
		malloc(sizeof(struct maze_markov_state_list));

	ret->node = s;
	ret->next = n;

	return ret;
}

void maze_markov_state_list_destroy(struct maze_markov_state_list *l)
{
	struct maze_markov_state_list *tmp;

	while((l) && (tmp = l->next))
	{
		maze_markov_state_remove(l->node);
		free(l);
		l = tmp;
	}
	
	if (l)
	{
		maze_markov_state_remove(l->node);
		free(l);	
	}
}

struct maze_markov_state *maze_markov_state_list_lookup(
	struct maze_markov_state_list *l,
	unsigned int x,
	unsigned int y
)
{
	while ((l) && ((l->node->coord.x != x) || (l->node->coord.y != y)))
		l = l->next;
	
	if (l)
		return l->node;
	else
		return 0;
}

#define MAZE_MARKOV_STATE_BUILD_LOOKUP(MZ,EXP,X,Y,FX,FY) __extension__ \
({ \
struct maze_markov_state *RET; \
if (!(RET = maze_markov_state_list_lookup((EXP),(X)+(FX),(Y)+(FY)))) \
	RET = maze_markov_state_build(MZ,(X)+(FX),(Y)+(FY),&(EXP)); \
RET; \
})


/* neighbourhood, state number, source state, transition list, action, fail ratio */
#define MAZE_MARKOV_STATE_BUILD_TRANSITIONS(NBH,S,SRC,TL,ACT,FR) \
if ((NBH[S])) \
{ \
	unsigned int EXP, I; \
	EXP = S; \
	for (I = 0; I < MAZE_MARKOV_ACTION_NB; I++) \
	{ \
		if ((I != EXP) && (NBH[I])) \
		{ \
			TL = maze_markov_transition_add((TL),NBH[I],ACT,FR); \
		} \
	} \
	if (!(TL)) \
	{ \
		TL = maze_markov_transition_add((TL),SRC,ACT,FR); \
	} \
	TL = maze_markov_transition_add((TL),NBH[S],ACT,MAZE_PROBA_MOVE_SUCCESS_RATIO); \
} \
else \
{ \
	TL = maze_markov_transition_add((TL),SRC,ACT,MAZE_PROBA_MOVE_GLOBAL_RATIO); \
} 

struct maze_markov_state *
maze_markov_state_build(
	struct maze *m,
	unsigned int x,
	unsigned int y,
	struct maze_markov_state_list **expect
)
{
	struct maze_markov_state *ret;

	if ((ret = maze_markov_state_list_lookup(*expect,x,y))) 
		0;
	else if ((!MAZE_IS_VALID_POSITION(m,x,y)) || (!MAZE_GET_SQUARE(m,x,y)))
		ret = 0;
	else
	{
		/* neighbourhood: up, down, left, right (in order) */
		struct maze_markov_state *nbh[MAZE_MARKOV_ACTION_NB];
		unsigned int stcnt, i;
		float fratio;

		ret = maze_markov_state_create(
			x,
			y,
			(MAZE_GET_SQUARE(m,x,y)->element 
				? MAZE_GET_SQUARE(m,x,y)->element->reward 
				: MAZE_DEFAULT_REWARD
			)
		);
		stcnt = 0;

		*expect = maze_markov_state_list_add(*expect,ret);
	
		nbh[0] = MAZE_MARKOV_STATE_BUILD_LOOKUP(m,*expect,x,y,0,1);
		nbh[1] = MAZE_MARKOV_STATE_BUILD_LOOKUP(m,*expect,x,y,0,-1);
		nbh[2] = MAZE_MARKOV_STATE_BUILD_LOOKUP(m,*expect,x,y,-1,0);
		nbh[3] = MAZE_MARKOV_STATE_BUILD_LOOKUP(m,*expect,x,y,1,0);

		for (i = 0; i < MAZE_MARKOV_ACTION_NB; i++)
		{
			if (nbh[i])
				stcnt++;
		}
	
		fratio = (MAZE_PROBA_MOVE_FAIL_RATIO 
			/ (stcnt - (stcnt > 1 ? 1 : 0)));

		MAZE_MARKOV_STATE_BUILD_TRANSITIONS(
			nbh,0,ret,ret->up,MAZE_MARKOV_ACTION_UP,fratio
		);
		MAZE_MARKOV_STATE_BUILD_TRANSITIONS(
			nbh,1,ret,ret->down,MAZE_MARKOV_ACTION_DOWN,fratio
		);
		MAZE_MARKOV_STATE_BUILD_TRANSITIONS(
			nbh,2,ret,ret->left,MAZE_MARKOV_ACTION_LEFT,fratio
		);
		MAZE_MARKOV_STATE_BUILD_TRANSITIONS(
			nbh,3,ret,ret->right,MAZE_MARKOV_ACTION_RIGHT,fratio
		);
	}

	return ret;
}


struct maze_markov_transition_list *
maze_markov_transition_add(
	struct maze_markov_transition_list *t,
	struct maze_markov_state *dest,
	enum maze_markov_action action,
	float proba
)
{
	struct maze_markov_transition_list *ret;

	ret = (struct maze_markov_transition_list *)
		malloc(sizeof(struct maze_markov_transition_list));

	ret->next = t;
	ret->dest = dest;
	ret->action = action;
	ret->probability = proba;

	return ret;
}

void maze_markov_transition_destroy(struct maze_markov_transition_list *t)
{
	struct maze_markov_transition_list *tmp;
	
	while ((t) && (tmp = t->next))
	{
		free(t);
		t = tmp;
	}

	if (t)
		free(t);
}

void maze_markov_decision_process_display(struct maze_markov_decision_process *mdp)
{
	printf("Markov Decision Process:\n");
	printf("S = {");
	maze_markov_state_display(mdp->states);
	printf("\n}\n"
		"A = {\n\t%s,\n\t%s,\n\t%s,\n\t%s\n}\n"
		"T = {",
		MAZE_MARKOV_ACTION_NAMES(MAZE_MARKOV_ACTION_UP), 
		MAZE_MARKOV_ACTION_NAMES(MAZE_MARKOV_ACTION_DOWN), 
		MAZE_MARKOV_ACTION_NAMES(MAZE_MARKOV_ACTION_LEFT), 
		MAZE_MARKOV_ACTION_NAMES(MAZE_MARKOV_ACTION_RIGHT) 
	);
	maze_markov_transition_display(mdp->states);
	printf("\n}\nR = {");
	maze_markov_reward_display(mdp->states);
 
	printf("\n}\n---\n");
}

void maze_markov_state_display(struct maze_markov_state_list *l)
{
	do
	{
		printf("\n\t%d: (%d,%d)",
			l->node->id, l->node->coord.x,l->node->coord.y);

		l = l->next;
	} while (l);
}

#define MAZE_MARKOV_TRANSITION_LIST_DISPLAY(L,SRC) \
if (L) \
{ \
	do \
	{ \
		printf("\n\t(%d,%s,%d) = %g",(SRC)->id,MAZE_MARKOV_ACTION_NAMES((L)->action),(L)->dest->id,(L)->probability); \
		(L) = (L)->next; \
	} while ((L)); \
}
	
void maze_markov_transition_display(struct maze_markov_state_list *l)
{
	struct maze_markov_transition_list *tmp;

	do
	{
		tmp = l->node->up;
		MAZE_MARKOV_TRANSITION_LIST_DISPLAY(tmp,l->node);
		tmp = l->node->down;
		MAZE_MARKOV_TRANSITION_LIST_DISPLAY(tmp,l->node);
		tmp = l->node->left;
		MAZE_MARKOV_TRANSITION_LIST_DISPLAY(tmp,l->node);
		tmp = l->node->right;
		MAZE_MARKOV_TRANSITION_LIST_DISPLAY(tmp,l->node);
		
		l = l->next;
	} while (l);
}


void maze_markov_reward_display(struct maze_markov_state_list *l)
{
	do
	{
		printf("\n\t%d: %g",l->node->id, l->node->reward);

		l = l->next;
	} while (l);
}

enum maze_markov_action maze_markov_get_action_random()
{
	static enum maze_markov_action possibleact[] = {
		MAZE_MARKOV_ACTION_UP,
		MAZE_MARKOV_ACTION_DOWN,
		MAZE_MARKOV_ACTION_LEFT,
		MAZE_MARKOV_ACTION_RIGHT
	};

	return possibleact[(rand() % MAZE_MARKOV_ACTION_NB)];
}
