#ifndef _MAZE_MATH_H
#define _MAZE_MATH_H

#define MAXF4(V1,V2,V3,V4) __extension__ \
({ \
	float RET; \
	if (((V1) <= (V2)) && ((V1) <= (V3)) && ((V1) <= (V4))) \
		ret = (V1); \
	else if (((V2) <= (V1)) && ((V2) <= (V3)) && ((V2) <= (V4))) \
		ret = (V2); \
	else if (((V3) <= (V1)) && ((V3) <= (V1)) && ((V3) <= (V4))) \
		ret = (V3); \
	else if (((V4) <= (V1)) && ((V4) <= (V2)) && ((V4) <= (V3))) \
		ret = (V4); \
	ret; \
})

#endif
