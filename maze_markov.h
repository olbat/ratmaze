#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "maze.h"

#define MAZE_MARKOV_ACTION_NB 4
enum maze_markov_action
{
	MAZE_MARKOV_ACTION_UP,
	MAZE_MARKOV_ACTION_DOWN,
	MAZE_MARKOV_ACTION_LEFT,
	MAZE_MARKOV_ACTION_RIGHT
};

struct maze_markov_decision_process
{
	struct maze_markov_state *init;
	struct maze_markov_state_list *states;
};

struct maze_markov_transition_list
{
	struct maze_markov_state *dest;
	enum maze_markov_action action;
	float probability;

	struct maze_markov_transition_list *next;
};

/* Each state transition has the same cost */
struct maze_markov_state
{
	unsigned int id;
	struct point coord;
	float reward;
	
	struct maze_markov_transition_list *up;
	struct maze_markov_transition_list *down;
	struct maze_markov_transition_list *left;
	struct maze_markov_transition_list *right;
};

struct maze_markov_state_list
{
	struct maze_markov_state *node;
	struct maze_markov_state_list *next;
};

struct maze_markov_decision_process *maze_markov_decision_process_create(struct maze *m);
void maze_markov_decision_process_destroy(struct maze_markov_decision_process *mdp);

struct maze_markov_state *maze_markov_state_create(unsigned int x, unsigned int y, float reward);
void maze_markov_state_remove(struct maze_markov_state *s);

struct maze_markov_state_list *maze_markov_state_list_add(struct maze_markov_state_list *n,struct maze_markov_state *s);
void maze_markov_state_list_destroy(struct maze_markov_state_list *l);
struct maze_markov_state *maze_markov_state_list_lookup(struct maze_markov_state_list *l,unsigned int x,unsigned int y);

struct maze_markov_state *maze_markov_state_build(struct maze *m,unsigned int x,unsigned int y,struct maze_markov_state_list **expect);

struct maze_markov_transition_list *maze_markov_transition_add(struct maze_markov_transition_list *t, struct maze_markov_state *dest, enum maze_markov_action action, float proba);
void maze_markov_transition_destroy(struct maze_markov_transition_list *t);

void maze_markov_decision_process_display(struct maze_markov_decision_process *mdp);
void maze_markov_state_display(struct maze_markov_state_list *l);
void maze_markov_transition_display(struct maze_markov_state_list *l);
__inline__ const char *maze_markov_action_names(enum maze_markov_action a);
void maze_markov_reward_display(struct maze_markov_state_list *l);

#endif
