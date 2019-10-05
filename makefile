CC = gcc
OBJS = main.o ErrorHandler.o ExhaustiveBacktracker.o FileManage.o Game.o List.o LP.o MainAux.o Parser.o ParserAux.o Stack.o Execution.o
PROJ_FILES = main.c ErrorHandler.c ErrorHandler.h ExhaustiveBacktracker.c ExhaustiveBacktracker.h FileManage.c FileManage.h Game.c Game.h List.c List.h LP.c LP.h MainAux.c MainAux.h Parser.c Parser.h ParserAux.c ParserAux.h SPBufferset.h Stack.c Stack.h Execution.c Execution.h
EXEC = sudoku-console
LOGS = gur.log gur.lp
MAKE_FILE = makefile
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c ErrorHandler.h ExhaustiveBacktracker.h FileManage.h Game.h List.h LP.h MainAux.h Parser.h ParserAux.h Stack.h SPBufferset.h
	$(CC) $(COMP_FLAG) -c $*.c
ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	$(CC) $(COMP_FLAG) -c $*.c
ExhaustiveBacktracker.o: ExhaustiveBacktracker.c ExhaustiveBacktracker.h
	$(CC) $(COMP_FLAG) -c $*.c
FileManage.o: FileManage.c FileManage.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h
	$(CC) $(COMP_FLAG) -c $*.c
List.o: List.c List.h
	$(CC) $(COMP_FLAG) -c $*.c
MainAux.o: MainAux.c MainAux.h
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
ParserAux.o: ParserAux.c ParserAux.h
	$(CC) $(COMP_FLAG) -c $*.c
Stack.o: Stack.c Stack.h
	$(CC) $(COMP_FLAG) -c $*.c
Execution.o: Execution.c Execution.h
	$(CC) $(COMP_FLAG) -c $*.c
LP.o: LP.c LP.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC) ${LOGS} 
