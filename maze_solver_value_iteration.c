#include "maze_solver_value_iteration.h"

struct maze_solver_vi_node *maze_solver_vi_node_create(
	struct maze_markov_state *state,
	struct maze_solver_vi_node *prec
)
{
	struct maze_solver_vi_node *ret;

	ret = (struct maze_solver_vi_node *) 
		malloc(sizeof(struct maze_solver_vi_node));

	ret->state = state;

}
