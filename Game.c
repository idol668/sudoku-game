/* Game module - The module contains auxiliary functions for the  management
 * of the game such as creating a new game or freeing one,checking the validity of value,
 *  checking whether the board wins, finding possible values for cell/row/column/block, updating
 *  the erroneous board after changes and more.
 */

# include "Game.h"
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "List.h"
#include "ErrorHandler.h"
/*This func initiate a game with a game board with m*n sized blocks ,two boards for fixed and erroneous cells
 * and undo/redo list structure using newList. */
void newGame(Game ** PPgame, int m, int n) {
	Game * gameAttmpt = calloc(1, sizeof(Game));
	if (gameAttmpt == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	gameAttmpt->m = m;
	gameAttmpt->n = n;
	gameAttmpt->N = m * n;
	gameAttmpt->gameBoard = calloc(gameAttmpt->N * gameAttmpt->N, sizeof(int));
	gameAttmpt->fixedBoard = calloc(gameAttmpt->N * gameAttmpt->N, sizeof(int));
	gameAttmpt->erroneousValues = calloc(gameAttmpt->N * gameAttmpt->N, sizeof(int));
	if (gameAttmpt->gameBoard == NULL || gameAttmpt->fixedBoard == NULL
			|| gameAttmpt->erroneousValues == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	newList(&(gameAttmpt->moveList));
	(*PPgame) = gameAttmpt;
}
/*This func free game board ,erroneous board and fixed cells board ,as well as the undo/redo list using freeList. */
void freeGame(Game* game) {
	if (game != NULL) {
		free(game->gameBoard);
		free(game->fixedBoard);
		free(game->erroneousValues);
		freeList(&(game->moveList));
		free(game);
	}
}
/* This func returns an address to copy of the array oldBoard with size boardSize.*/
int* copyBoard(int * oldBoard, int boardSize) {
	int i;
	int *newBoard = calloc(boardSize, sizeof(int));
	if (newBoard == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	for (i = 0; i < boardSize; i++) {
		newBoard[i] = oldBoard[i];
	}
	return newBoard;
}

/*THis func returns 1 if all the cells in the game board are filled and 0 otherwise.*/
int isComplete(Game* game) {
	int i, j;
	for (i = 0; i < game->N; i++) {
		for (j = 0; j < game->N; j++) {
			if (game->gameBoard[i * game->N + j] == 0) {
				return 0;
			}
		}
	}
	return 1;
}
/*This func returns 1 if cell in position x,y is valid with respect to his neighbors and 0 otherwise.when forFix ==1 only the validity of fixed cell will be checked.  */
int cellIsValid(Game* game, int x, int y, int forFix) {
	int row, col;
	if (forFix == 0 && game->gameBoard[x * game->N + y] == 0) {
		return 1;
	}
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			if (((x == row || y == col
					|| (((x / game->m) == (row / game->m)) && ((y / game->n) == (col / game->n))))
					&& game->gameBoard[row * game->N + col] == game->gameBoard[x * game->N + y]
					&& (x != row || y != col))
					&& (forFix == 0 || (forFix == 1 && game->fixedBoard[row * game->N + col] == 1))) {
				return 0;
			}
		}
	}
	return 1;
}
/*This func check if the solution to the game board is correct using cellIsValid func.*/
int correctSolution(Game* game) {
	int row, col;
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			if (cellIsValid(game, row, col, 0) == 0) {
				return 0;
			}
		}
	}
	return 1;
}
/*This func update erroneous board from changes that occurred by changing cell x,y in game board,while also
 * creating a list of moves for the undo/redo list.
 if (x==-1 && y==-1) the func update erroneous board from changes that occurred in game board*/
void updateErroneuos(Game *game, int x, int y, Move *** PfixedBoardMoves,
		int ** numOfFixBoardChanges) {
	int flagCheckBoard = 0;
	int row, col;
	if (numOfFixBoardChanges != NULL) {
		(**numOfFixBoardChanges) = 0;
	}
	if (x == -1 && y == -1) {/*if flagcCheckBoard is 1 check all non fixed cells*/
		flagCheckBoard = 1;
	}
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			if (((flagCheckBoard == 0
					&& ((x == row || y == col
							|| (((x / game->m) == (row / game->m))
									&& ((y / game->n) == (col / game->n))))))
					|| (flagCheckBoard == 1)) && (game->fixedBoard[row * game->N + col] == 0)) {/*check x,y neighbors or all non fixed cells*/
				if (cellIsValid(game, row, col, 0) == 1) {/*cell valid*/
					if (PfixedBoardMoves != NULL
							&& game->erroneousValues[row * game->N + col] == 1) {/*create node for undo/redo list*/
						(**numOfFixBoardChanges)++;
						(*PfixedBoardMoves) = realloc((*PfixedBoardMoves),
								(**numOfFixBoardChanges) * sizeof(Move*));
						newMove(&(*PfixedBoardMoves)[(**numOfFixBoardChanges) - 1], row, col, 1, 0);
					}
					game->erroneousValues[row * game->N + col] = 0;
				} else {/*cell invalid*/
					if (PfixedBoardMoves != NULL
							&& game->erroneousValues[row * game->N + col] == 0) {/*create node for undo/redo list*/
						(**numOfFixBoardChanges)++;
						(*PfixedBoardMoves) = realloc((*PfixedBoardMoves),
								(**numOfFixBoardChanges) * sizeof(Move*));
						newMove(&(*PfixedBoardMoves)[(**numOfFixBoardChanges) - 1], row, col, 0, 1);
					}
					game->erroneousValues[row * game->N + col] = 1;
				}
			}
		}
	}
}

/*This func checks the validity of fixed cells using cellIsValid.*/
int erroneousFixCheck(Game* game) {
	int row, col;
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			if (game->fixedBoard[row * game->N + col] == 1) {
				if (cellIsValid(game, row, col, 1) == 0) {
					return 1;
				}
			}
		}
	}
	return 0;
}
/*This func create an array of possible values from illigaleValues array by adding all values
 * that their index in illigaleVales is 0 to possible values array and assign it to *optinalValuesPoniter. */
void updateOptinalValues(int numOfValidValues, int **optinalValuesPoniter, int* illigaleValues,
		int N) {
	int index = 0, i = 0;
	if (numOfValidValues == 0) {
		*optinalValuesPoniter = NULL;
	} else {
		*optinalValuesPoniter = calloc(numOfValidValues, sizeof(int));
		if (*optinalValuesPoniter == NULL) {
			printError(MemoryFailed);
			printDetails(exiting);
			exit(0);
		}
		for (i = 0; i < N; i++) {
			if (illigaleValues[i] == 0) {
				(*optinalValuesPoniter)[index] = i + 1;
				index++;
			}
		}
	}
	free(illigaleValues);
}

/*This func create an illegalVales array for cell in row,col position that has the value 1 at the index of each illegalValue
 * and uses updateOptinalValues for creating the array of possible value. */
int possiableValuesforCell(int *gameBoard, int m, int n, int row, int col,
		int **optinalValuesPoniter) {
	int i, j, N = (m * n), numOfValidValues = N;
	int* illigaleValues = calloc(N, sizeof(int));
	if (illigaleValues == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (gameBoard[i * N + j] != 0
					&& (i == row || j == col || (((i / m) == (row / m)) && ((j / n) == (col / n))))) {
				if (illigaleValues[gameBoard[i * N + j] - 1] == 0) {/*mark value as illegal*/
					numOfValidValues--;
				}
				illigaleValues[gameBoard[i * N + j] - 1] = 1;
			}
		}
	}
	updateOptinalValues(numOfValidValues, optinalValuesPoniter, illigaleValues, N);
	return numOfValidValues;
}
/*This func create an illegalVales array for row or column (depending on enum neighbor) that has the value 1 at the index of each illegal value
 * and uses updateOptinalValues for creating the array of possible value. */
int possiableValuesforColOrRow(int *gameBoard, int m, int n, int rowOrCol,
		int **optinalValuesPoniter, Neighbor neighbor) {
	int j, N = (m * n), numOfValidValues = N;
	int* illigaleValues = calloc(N, sizeof(int));
	if (illigaleValues == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	for (j = 0; j < N; j++) {
		if (neighbor == column && gameBoard[j * N + rowOrCol] != 0) {/*mark value as illegal*/
			numOfValidValues--;
			illigaleValues[gameBoard[j * N + rowOrCol] - 1] = 1;
		} else if (neighbor == rows && gameBoard[rowOrCol * N + j] != 0) {/*mark value as illegal*/
			numOfValidValues--;
			illigaleValues[gameBoard[rowOrCol * N + j] - 1] = 1;
		}
	}
	updateOptinalValues(numOfValidValues, optinalValuesPoniter, illigaleValues, N);
	return numOfValidValues;
}
/*This func create an illegalVales array for block that has the value 1 at the index of each illegal value
 * and uses updateOptinalValues for creating the array of possible value. */
int possiableValuesforBlock(int *gameBoard, int m, int n, int block, int **optinalValuesPoniter) {
	int N = (m * n), numOfValidValues = N, blkRow, blkCol;
	int* illigaleValues = calloc(N, sizeof(int));
	if (illigaleValues == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	for (blkRow = 0; blkRow < m; blkRow++) {
		for (blkCol = 0; blkCol < n; blkCol++) {
			if (gameBoard[((block / m) * m + blkRow) * N + (block % m) * n + blkCol] != 0) {/*mark value as illegal*/
				numOfValidValues--;
				illigaleValues[gameBoard[((block / m) * m + blkRow) * N + (block % m) * n + blkCol]
						- 1] = 1;
			}
		}
	}
	updateOptinalValues(numOfValidValues, optinalValuesPoniter, illigaleValues, N);
	return numOfValidValues;
}
/*This func return 1 if there's an erroneous cell ,by checking the erroneous board and 0 otherwise.*/
int isErroneous(Game * game) {
	int i, j;
	for (i = 0; i < game->N; i++) {
		for (j = 0; j < game->N; j++) {
			if (game->erroneousValues[i * game->N + j] == 1) {
				return 1;
			}
		}
	}
	return 0;
}
/*This func return the number of empty cells in the game board.*/
int numOfEmptyCells(Game * game) {
	int i, j, res = 0;
	for (i = 0; i < game->N; i++) {
		for (j = 0; j < game->N; j++) {
			if (game->gameBoard[i * game->N + j] == 0) {
				res++;
			}
		}
	}
	return res;
}
/*This func checks if the game is won, using isComplete to check if the game board is complete ,
 * and correctSolution to check the solution validity. if the board is full an appropriate message will be printed.*/
void isWinner(Game** PPGame, int* gameMode) {
	if (isComplete((*PPGame)) && (*gameMode == 2)) {
		if (correctSolution((*PPGame))) {
			printf("Congratulations , you solved the sudoku puzzle\n");
			(*gameMode) = 0;
		} else {
			printf("The solution is erroneous\n");
		}
	}
}
