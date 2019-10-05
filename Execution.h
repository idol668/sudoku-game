/*Execution module - responsible for the execution of various commend in the game */
#ifndef EXECUTION_H_
#define EXECUTION_H_

void solveMode(Game ** PPGame, int * gameMode, int * markErrors,
		char * firstArg);
void editMode(Game ** PPGame, int * gameMode, int * markErrors, char * firstArg);
void markErrros(int * markErrors, char * firstArg);
void set(Game ** PPGame, int * gameMode, int * markErrors, char * firstArg,
		char * secondArg, char * thirdArg);
void guess(Game ** PPGame, int * gameMode, int * markErrors, float threshold);
void generate(Game ** PPGame, int * gameMode, int * markErrors, int intX,
		int intY, int emptyCells, int * tempBoard, int numOfMoves,
		Move ** moves);
void undo(Game ** PPGame, int * gameMode, int * markErrors);
void redo(Game ** PPGame, int * gameMode, int * markErrors);
void hint(Game** PPGame,int intX,int intY);
void autofill(Game ** PPGame, int * gameMode, int * markErrors);
void reset(Game ** PPGame, int * gameMode, int * markErrors);

#endif /* EXECUTION_H_ */
