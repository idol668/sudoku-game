/* Game module - The module contains auxiliary functions for the  management
 * of the game such as creating a new game or freeing one,checking the validity of value,
 *  checking whether the board wins, finding possible values for cell/row/column/block, updating
 *  the erroneous board after changes, etc.
 */

#ifndef GAME_H_
#define GAME_H_
#include "List.h"
/* "game" struct - contain information about the size, gameboard,fixedboard ,erronrousvalus board , moveList
 * about current game (for redo/undo ) .*/
typedef struct game {
	int n,m,N;
	int* gameBoard;
	int* fixedBoard;
	int* erroneousValues;
	List moveList;
} Game;

/*Enum Neighbor -  So that we can differentiate between the neighbor types in a cell validity check function */
typedef enum neighbor {
	column, rows, blocks
} Neighbor;
void newGame(Game ** PPgame, int m, int n);
void freeGame(Game *game);
int correctSolution(Game* game);
int isComplete(Game* game);
void updateErroneuos(Game *game, int x, int y, Move *** PfixedBoardMoves,
		int ** numOfFixBoardChanges);
int* copyBoard(int * oldBoard, int boardSize);
int possiableValuesforCell(int *gameBoard, int m, int n, int row, int col,
		int **optinalValuesPoniter);
int possiableValuesforColOrRow(int *gameBoard, int m, int n, int roworcol,
		int **optinalValuesPoniter, Neighbor neighborg);
int possiableValuesforBlock(int *gameBoard, int m, int n, int block,
		int **optinalValuesPoniter);
int isErroneous(Game * game);
int numOfEmptyCells(Game * game);
int erroneousFixCheck(Game* game);
void isWinner(Game** PPGame, int* gameMode);
#endif /* GAME_H_ */
