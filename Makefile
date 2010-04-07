CFLAGS= -Wall -ansi -pedantic -O3 -fomit-frame-pointer -g
CC= gcc ${CFLAGS} 
SNAME= ratmaze
SRC=maze.c maze_markov.c maze_markov_bellman.c maze_solver_value_iteration.c maze_solver_policy_iteration.c maze_solver_qlearning.c ratmaze.c
OBJ=$(SRC:.c=.o)

all : ${SNAME} clean
	@echo success making ${SNAME}
dev : ${SNAME}
	@echo success making ${SNAME}
${SNAME} : ${OBJ}
	@echo making ${SNAME}
	@${CC} -o $@ ${OBJ}
%.o : %.c
	@echo -n 'compiling $< ... '
	@${CC} -o $@ -c $<
	@echo done
clean :
	@echo cleaning object files
	@rm -f ${OBJ}
cleanall : clean
	@echo cleaning executable file
	@rm -f ${SNAME}
