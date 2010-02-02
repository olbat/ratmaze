CFLAGS= -Wall -ansi -pedantic -O3 -fomit-frame-pointer -g
CC= gcc ${CFLAGS} 
SNAME= maze
SRC= maze.c
OBJ= $(SRC:.c=.o)

all : ${SNAME} clean
	@echo success making ${SNAME}
dev : ${SNAME}
	@echo success making ${SNAME}
%.o : %.c
	@echo -n 'compiling $< ... '
	@{$CC} -o $@ -c $<
	@echo done
clean :
	@echo cleaning object files
	@rm -f ${OBJ}
cleanall : clean
	@echo cleaning executable file
	@rm -f ${SNAME}
