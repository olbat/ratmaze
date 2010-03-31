#include "maze.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENT_SET_COORD(E,X,Y) __extension__ \
({ \
	(E)->coord.x = (X); \
	(E)->coord.y = (Y); \
})


struct maze *maze_create(unsigned int width, unsigned int height)
{
	struct maze *ret;

	ret = (struct maze *) malloc(sizeof(struct maze));

	ret->width = width;
	ret->height = height;

	ret->table = (struct square ***) malloc(width*sizeof(struct square **));
	memset((char *)ret->table,0,width*sizeof(struct square **));

	while (width--)
	{
		*(ret->table + width) = (struct square **) 
			malloc(height*sizeof(struct square *));
		memset(
			(char *) *(ret->table + width),
			0,
			height*sizeof(struct square *)
		);
	}

	return ret;
}

void maze_delete(struct maze *m)
{
	unsigned int x, y;

	x = m->width;
	while (x--)
	{
		y = m->height;
		while (y--)
		{
			if (MAZE_GET_SQUARE(m,x,y))
				square_free(MAZE_GET_SQUARE(m,x,y));
		}
		free(*(m->table + x));
	}

	free(m->table);
	free(m);
}

struct square *
maze_add(struct maze *m, struct square *s, unsigned int x, unsigned int y)
{
	struct square *ret;

	if (MAZE_GET_SQUARE(m,x,y))
		maze_remove(m,x,y);

	MAZE_GET_SQUARE(m,x,y) = ret = square_duplicate(s);

	if (ret->element)
		ELEMENT_SET_COORD(ret->element,x,y);
	
	return ret;
}

void maze_remove(struct maze *m, unsigned int x, unsigned int y)
{
	square_free(MAZE_GET_SQUARE(m,x,y));
	MAZE_GET_SQUARE(m,x,y) = 0;
}

/* >>> TODO: do a macro if necessary */
struct square *square_duplicate(struct square *s)
{
	struct square *ret;

	ret = (struct square *) malloc(sizeof(struct square));
	memcpy(ret,s,sizeof(struct square));

	/* >>> TODO: duplicate the point if necessary */
	if (s->element)
	{
		ret->element = (struct element *) malloc(sizeof(struct element));
		memcpy(ret->element,s->element,sizeof(struct element));
	}
	else
		ret->element = 0;

	return ret;
}

void square_free(struct square *s)
{
	free(s->element);
	free(s);
}

void maze_display(struct maze *m)
{
	unsigned int x, y;
	__extension__ char linebuff[m->width + 1];

	linebuff[m->width] = 0;
	y = m->height;

	while (y--)
	{
		x = m->width;
		while (x--)
		{
			if (MAZE_GET_SQUARE(m,x,y))
			{
				if ((MAZE_GET_SQUARE(m,x,y))->element)
				{
					linebuff[x] = 
					(MAZE_GET_SQUARE(m,x,y))->element->name;
				}
				else
					linebuff[x] = MAZE_DISPLAY_EMPTY_SQUARE;
			}
			else
				linebuff[x] = MAZE_DISPLAY_NO_SQUARE;	
		}
		printf("%s\n",linebuff);
	}
}
