#ifndef _MAZE_SOLVER_QLEARNING_H
#define _MAZE_SOLVER_QLEARNING_H

#define MAZE_SOLVER_QLEARNING_DEFAULT_Q0 1.0f
#define MAZE_SOLVER_QLEARNING_DEFAULT_ALPHA 1.0f

#include "maze_markov.h"

void maze_solver_qlearning_perform(struct maze_markov_decision_process *mdp, unsigned int limit);

#endif
