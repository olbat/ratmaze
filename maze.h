#ifndef _MAZE_H
#define _MAZE_H

#define MAZE_DISPLAY_EMPTY_SQUARE 'o'
#define MAZE_DISPLAY_NO_SQUARE '.'


enum element_type
{
	MAZE_ELEM_TYPE_RAT,
	MAZE_ELEM_TYPE_CHEESE
};

struct point
{
	unsigned int x;
	unsigned int y;
};

struct element
{
	char name;
	struct point coord;
	enum element_type type;
};

struct square
{
	struct element *element;
};

struct maze
{
	unsigned int height;
	unsigned int width;
	struct square ***table;
};


struct maze *maze_create(unsigned int width, unsigned int height);
void maze_delete(struct maze *m);
struct square *maze_add(struct maze *m, struct square *s, unsigned int x, unsigned int y);
void maze_remove(struct maze *m, unsigned int x, unsigned int y);
void maze_display(struct maze *m);
struct square *square_duplicate(struct square *s);
void square_free(struct square *s);

#endif
