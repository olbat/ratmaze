#ifndef _MAZE_H
#define _MAZE_H

#define MAZE_DISPLAY_EMPTY_SQUARE 'o'
#define MAZE_DISPLAY_NO_SQUARE '.'



#define ELEMENT_TRANSLATE(E,TX,TY) __extension__ \
({ \
	ELEMENT_SET_COORD((E),(E)->coord.x + (TX),(E)->coord.y + (TY)); \
})

#define MAZE_IS_VALID_POSITION(M,X,Y) \
	(((X) >= 0) && ((X) < (M)->width) && ((Y) >= 0) && ((Y) < (M)->height))

#define MAZE_IS_GOAL(M,X,Y) \
	((MAZE_GET_SQUARE(M,X,Y)->element) && (MAZE_GET_SQUARE(M,X,Y)->element->type == MAZE_ELEM_TYPE_GOAL))

#define MAZE_MOVE_ELEMENT(M,SX,SY,DX,DY) __extension__ \
({ \
	/* \
	if ((DX >= 0) && (DX < (M)->width) && (DY >= 0) && (DY < (M)->height)) \
	{ \
	*/ \
		(MAZE_GET_SQUARE((M),(DX),(DY)))->element = \
			(MAZE_GET_SQUARE((M),(SX),(SY)))->element; \
		(MAZE_GET_SQUARE((M),(SX),(SY)))->element = 0; \
	/* \
	} \
	*/ \
})

#define MAZE_ELEMENT_TRANSLATE(M,E,TX,TY) __extension__ \
({ \
	if (MAZE_IS_VALID_POSITION(M,(E)->coord.x + (TX),(E)->coord.y + (TY))) \
	{ \
		MAZE_MOVE_ELEMENT(M,(E)->coord.x,(E)->coord.y, \
			(E)->coord.x + (TX), (E)->coord.y + (TY)); \
		ELEMENT_TRANSLATE(E,TX,TY); \
	} \
})
#define MAZE_ELEMENT_MOVE_UP(M,E) MAZE_ELEMENT_TRANSLATE(M,E,0,1)
#define MAZE_ELEMENT_MOVE_DOWN(M,E) MAZE_ELEMENT_TRANSLATE(M,E,0,-1)
#define MAZE_ELEMENT_MOVE_LEFT(M,E) MAZE_ELEMENT_TRANSLATE(M,E,-1,0)
#define MAZE_ELEMENT_MOVE_RIGHT(M,E) MAZE_ELEMENT_TRANSLATE(M,E,1,0)

#define MAZE_GET_SQUARE(M,X,Y) *(*((M)->table + (X)) + (Y))
#define MAZE_GET_UP_SQUARE(M,X,Y) MAZE_GET_SQUARE(M,X,(Y)+1) 
#define MAZE_GET_DOWN_SQUARE(M,X,Y) MAZE_GET_SQUARE(M,X,(Y)-1) 
#define MAZE_GET_LEFT_SQUARE(M,X,Y) MAZE_GET_SQUARE(M,(X)-1,Y) 
#define MAZE_GET_RIGHT_SQUARE(M,X,Y) MAZE_GET_SQUARE(M,(X)+1,Y) 



enum element_type
{
	MAZE_ELEM_TYPE_RAT,
	MAZE_ELEM_TYPE_GOAL
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
