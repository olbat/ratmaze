#ifndef _MAZE_MARKOV_PROCESS_H
#define _MAZE_MARKOV_PROCESS_H

struct maze_markov_process_value
{
	struct maze_markov_state *state;
	float cost;
};

#endif
