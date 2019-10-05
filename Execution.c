/*Execution module - responsible for the execution of various commend in the game such as entering solve or edit mode ,set values to the game board,
 * generating game board,undo/redo commands and more . */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MainAux.h"
#include "FileManage.h"
#include "ParserAux.h"
#include "Game.h"
#include "LP.h"
#include "ExhaustiveBacktracker.h"
#include "List.h"
#include "ErrorHandler.h"

#define generateMaxNunOfTries 1000

/*This func load a new game for solve mode from file (file path is firstArg) using loadGame function,when succeeded it changes the game mode to solve,
 *update the erroneous board using updateErroneous, print the game board and check if the game is finished using isWinner. */
void solveMode(Game **PPGame, int *gameMode, int *markErrors, char *firstArg) {
	if (loadGame(PPGame, firstArg, 2) == 1) {
		(*gameMode) = 2;
		updateErroneuos(*PPGame, -1, -1, NULL, NULL);
		printGameBoard(*PPGame, *gameMode, *markErrors);
		isWinner(PPGame, gameMode);
	}
}
/*This func load game for edit mode,  if firstArg==null an empty  9*9 board game is initialized, else load a game from file (file path is firstArg) using loadGame,
 * when succeeded it changes the game mode to solve,and  update the erroneous board using updateErroneous print the game board. */
void editMode(Game **PPGame, int *gameMode, int *markErrors, char *firstArg) {
	if (firstArg == NULL) {/*load 9*9 game when no file path provided */
		(*gameMode) = 1;
		freeGame(*PPGame);
		newGame(PPGame, 3, 3);
		printGameBoard((*PPGame), *gameMode, *markErrors);
	} else {
		if (loadGame(PPGame, firstArg, 1) == 1) {
			(*gameMode) = 1;
			updateErroneuos(*PPGame, -1, -1, NULL, NULL);
			printGameBoard((*PPGame), *gameMode, *markErrors);
		}
	}
}
/* This func changes the value of mark errors ,and check user input using isDigit.*/
void markErrros(int *markErrors, char *firstArg) {
	int val = isInteger(firstArg, 0);
	if (val == 1 || val == 0) {
		(*markErrors) = val;
	} else {
		printError(WMarkErrors);
	}
}
/*This func execute set on the game board,checking the parameters and board state validity ,set the value to the board while adding
 * the changes for the board and erroneous values board to the undo/redo list. */
void set(Game **PPGame, int *gameMode, int *markErrors, char *firstArg,
		char *secondArg, char *thirdArg) {
	int col, row, intZ;
	if ((col = isInteger(firstArg, 1)) == -1
			|| (!inRange(col, 0, ((*PPGame)->N) - 1))) {
		printError(FirstNotInRange);
		printf(" in range from 1 to %d\n", (*PPGame)->N);
	} else if ((row = isInteger(secondArg, 1)) == -1
			|| (!inRange(row, 0, ((*PPGame)->N) - 1))) {
		printError(SecondNotInRange);
		printf(" in range from 1 to %d\n", (*PPGame)->N);
	} else if ((intZ = isInteger(thirdArg, 0)) == -1
			|| (!inRange(intZ, 0, (*PPGame)->N))) {
		printError(ThirdNotInRange);
		printf(" in range from 0 to %d\n", (*PPGame)->N);
	} else if ((*gameMode) == 2
			&& (*PPGame)->fixedBoard[row * (*PPGame)->N + col] == 1) {
		printError(CellIsFixed);
	} else {
		int numOfGameBoardMoves=0;
		Move **gameBoardMoves =NULL;
		int **PnumOfErroneusBoardChanges = calloc(1, sizeof(int*));
		Move ***erroneusBoardMoves = calloc(1, sizeof(Move**));
		(*PnumOfErroneusBoardChanges) = calloc(1, sizeof(int));
		(*erroneusBoardMoves) = NULL;
		if((*PPGame)->gameBoard[row * (*PPGame)->N + col] !=intZ){
			gameBoardMoves = calloc(1, sizeof(Move*));
			newMove(gameBoardMoves, row, col,
							(*PPGame)->gameBoard[row * (*PPGame)->N + col], intZ);
			numOfGameBoardMoves++;
			(*PPGame)->gameBoard[row * (*PPGame)->N + col] = intZ;
		}
		updateErroneuos((*PPGame), row, col, erroneusBoardMoves,
				PnumOfErroneusBoardChanges);
		addNodeFromCurr(&((*PPGame)->moveList), gameBoardMoves, numOfGameBoardMoves,
				*erroneusBoardMoves, **PnumOfErroneusBoardChanges);/*add node to undo/redo list*/
		free(*PnumOfErroneusBoardChanges);
		free(PnumOfErroneusBoardChanges);
		free(erroneusBoardMoves);
		printGameBoard((*PPGame), *gameMode, *markErrors);
		isWinner(PPGame, gameMode);
	}
}
/*guess a solution to the game board using LP, and filling cells with probably above the given threshold,
 *  while adding the changes for the board and erroneous values board to the undo/redo list.  */
void guess(Game **PPGame, int *gameMode, int *markErrors, float threshold) {
	int *tempBoard;
	int checkResult = 2;
	if ((checkResult = solve(*PPGame, &tempBoard, 1, threshold, 0, 0, 0))
			== 1) {
		Move **moves = NULL;
		int numOfMoves = 0, row, col;
		for (row = 0; row < (*PPGame)->N; row++) {
			for (col = 0; col < (*PPGame)->N; col++) {
				if ((*PPGame)->gameBoard[row * (*PPGame)->N + col]
						!= tempBoard[row * (*PPGame)->N + col]) {
					numOfMoves++;
					moves = realloc(moves, numOfMoves * sizeof(Move*));
					newMove(&(moves[numOfMoves - 1]), row, col,
							(*PPGame)->gameBoard[row * (*PPGame)->N + col],
							tempBoard[row * (*PPGame)->N + col]);/*add changes to list of moves*/
				}
			}
		}
		addNodeFromCurr(&((*PPGame)->moveList), moves, numOfMoves,
		NULL, 0);/*add node to undo/redo list*/
		free((*PPGame)->gameBoard);
		(*PPGame)->gameBoard = tempBoard;
		printGameBoard((*PPGame), *gameMode, *markErrors);
		isWinner(PPGame, gameMode);
	} else if (checkResult == 0) {
		printError(UnsolvableBoard);
	} else if (checkResult == -1) {
		printError(guessFailed);
	}
}
/*Generates a game board by randomly filling cellsToAdd empty cells with legal values,running ILP to solve the board ,and clearing all
 * but cellsTokeep cells while adding the changes for the board and erroneous values board to the undo/redo list.  */
void generate(Game **PPGame, int *gameMode, int *markErrors, int cellsToAdd,
		int cellsTokeep, int emptyCells, int *tempBoard, int numOfMoves,
		Move **moves) {
	int attemptNum, cellsToFill, cellIdx, numOfPosiblVals, failedCell,
			cellsToClear, clearIdx, row, col;
	int *possiableVals;
	Game *genAttempt;
	Game **PgenAttempt = &genAttempt;
	for (attemptNum = 0; attemptNum < generateMaxNunOfTries; attemptNum++) {/*crating new attempt until no errors*/
		failedCell = 0;
		newGame(PgenAttempt, (*PPGame)->m, (*PPGame)->n);
		free(genAttempt->gameBoard);
		genAttempt->gameBoard = copyBoard((*PPGame)->gameBoard,
				(*PPGame)->N * (*PPGame)->N);
		for (cellsToFill = 0; cellsToFill < cellsToAdd; cellsToFill++) {/*fill cells*/
			while (1) {/*finding empty cell*/
				cellIdx = rand() % ((*PPGame)->N * (*PPGame)->N);
				if (genAttempt->gameBoard[cellIdx] == 0) {
					break;
				}
			}
			row = cellIdx / (*PPGame)->N;
			col = cellIdx % (*PPGame)->N;
			numOfPosiblVals = possiableValuesforCell(genAttempt->gameBoard,
					(*PPGame)->m, (*PPGame)->n, row, col, &possiableVals);
			if (numOfPosiblVals == 0) {/*no possibles values for cell fail*/
				freeGame(genAttempt);
				failedCell = 1;
				free(possiableVals);
				break;
			} else {
				int valIdx = rand() % numOfPosiblVals;
				genAttempt->gameBoard[cellIdx] = possiableVals[valIdx];
			}
			free(possiableVals);
		}
		if (failedCell == 1) {
			continue;
		}
		if (cellsToAdd < emptyCells) {/*solve the board with added cells*/
			if (solve(genAttempt, &tempBoard, 2, 0, 0, 0, 1) != 1) {
				freeGame(genAttempt);
				continue;
			}
		} else {
			tempBoard = copyBoard((*PPGame)->gameBoard,
					(*PPGame)->N * (*PPGame)->N);

	}
	for (cellsToClear = 0;
			cellsToClear < ((((*PPGame)->N * (*PPGame)->N)) - cellsTokeep);
			cellsToClear++) {/*clear cells according to user input*/
		while (1) {
			clearIdx = rand() % ((*PPGame)->N * (*PPGame)->N);
			if (tempBoard[clearIdx] != 0) {
				tempBoard[clearIdx] = 0;
				break;
			}
		}
	}
	for (row = 0; row < (*PPGame)->N; row++) {/*adding changes to undo/redo list*/
		for (col = 0; col < (*PPGame)->N; col++) {
			if ((*PPGame)->gameBoard[row * (*PPGame)->N + col]
					!= tempBoard[row * (*PPGame)->N + col]) {
				numOfMoves++;
				moves = realloc(moves, numOfMoves * sizeof(Move*));
				newMove(&(moves[numOfMoves - 1]), row, col,
						(*PPGame)->gameBoard[row * (*PPGame)->N + col],
						tempBoard[row * (*PPGame)->N + col]);
			}
		}
	}
	addNodeFromCurr(&((*PPGame)->moveList), moves, numOfMoves,
	NULL, 0);
	freeGame(genAttempt);
	free((*PPGame)->gameBoard);
	(*PPGame)->gameBoard = tempBoard;
	printGameBoard((*PPGame), *gameMode, *markErrors);
	return;
}
printError(GenerationFailed);
}
/*undo a move ,using the undo/redo list to execute and print the changes. */
void undo(Game **PPGame, int *gameMode, int *markErrors) {
int move;
if ((*PPGame)->moveList.current->next->numOfGameBoardMoves > 0) {
	printf("The following changes occurred:\n");
}
for (move = 0; move < (*PPGame)->moveList.current->next->numOfGameBoardMoves;
		move++) {/*making and printing game board changes*/
	(*PPGame)->gameBoard[(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->x
			* (*PPGame)->N
			+ (*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->y] =
			(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->old;
	printf("cell %d,%d changed from %d to %d\n",
			(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->y
					+ 1,
			(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->x
					+ 1,
			(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->new,
			(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->old);
}
for (move = 0; move < (*PPGame)->moveList.current->next->numOfErrorBoardMoves;
		move++) {/*making erroneous game board changes*/
	(*PPGame)->erroneousValues[(*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->x
			* (*PPGame)->N
			+ (*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->y] =
			(*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->old;
}
if ((*PPGame)->moveList.current->next->numOfGameBoardMoves == 0
		&& (*PPGame)->moveList.current->next->numOfErrorBoardMoves == 0) {
	printf("There were no changes to be made.\n");
}
printGameBoard((*PPGame), *gameMode, *markErrors);
}
/*redo a move ,using the undo/redo list to execute and print the changes. */
void redo(Game **PPGame, int *gameMode, int *markErrors) {
int move;
if ((*PPGame)->moveList.current->numOfGameBoardMoves > 0) {
	printf("The following changes occurred:\n");
}
for (move = 0; move < (*PPGame)->moveList.current->numOfGameBoardMoves;
		move++) {/*making and printing game board changes*/
	(*PPGame)->gameBoard[(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->x
			* (*PPGame)->N
			+ (*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->y] =
			(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->new;
	printf("cell %d,%d changed from %d to %d\n",
			(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->y + 1,
			(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->x + 1,
			(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->old,
			(*PPGame)->moveList.current->ptrListOfGameBoardMoves[move]->new);
}
for (move = 0; move < (*PPGame)->moveList.current->numOfErrorBoardMoves;
		move++) {/*making erroneous game board changes*/
	(*PPGame)->erroneousValues[(*PPGame)->moveList.current->ptrListOfErrorBoardMoves[move]->x
			* (*PPGame)->N
			+ (*PPGame)->moveList.current->ptrListOfErrorBoardMoves[move]->y] =
			(*PPGame)->moveList.current->ptrListOfErrorBoardMoves[move]->new;
}
if ((*PPGame)->moveList.current->numOfGameBoardMoves == 0
		&& (*PPGame)->moveList.current->numOfErrorBoardMoves == 0) {
	printf("There were no changes to be made.\n");
}
printGameBoard((*PPGame), *gameMode, *markErrors);
}
/*showing the solution to the x,y cell ,using ILP to solve the board.*/
void hint(Game **PPGame, int row, int col) {
int checkResult = 2;
if ((*PPGame)->fixedBoard[row * (*PPGame)->N + col] == 1) {
	printError(CellIsFixed);
} else if ((*PPGame)->gameBoard[row * (*PPGame)->N + col] != 0) {
	printError(CellIsNotEmpty);
} else {
	int *solvedBoard = NULL;
	if ((checkResult = solve((*PPGame), &solvedBoard, 2, 0, 0, 0, 1)) == 1) {
		printf("Hint for cell %d,%d is %d\n", col + 1, row + 1,
				solvedBoard[row * (*PPGame)->N + col]);
		free(solvedBoard);
	} else if (checkResult == 0) {
		printError(UnsolvableBoard);
	} else if (checkResult == -1) {
		printError(hintFailed);
	}
}

}
/*fills all cell with one possible value in the current board   while adding the changes
 * for the game board and erroneous values board to the undo/redo list*/
void autofill(Game **PPGame, int *gameMode, int *markErrors) {
int row, col, numOfPsbVals, numOfMoves = 0, *lstOfPsbVal, *autofillGameBoard;
int **PnumOfErroneusBoardChanges = calloc(1, sizeof(int*));
Move ***erroneusBoardMoves = calloc(1, sizeof(Move**));
Move **moves = NULL;
(*PnumOfErroneusBoardChanges) = calloc(1, sizeof(int));
autofillGameBoard = copyBoard((*PPGame)->gameBoard,
		(*PPGame)->N * (*PPGame)->N);
for (row = 0; row < (*PPGame)->N; row++) {
	for (col = 0; col < (*PPGame)->N; col++) {
		if ((*PPGame)->gameBoard[row * (*PPGame)->N + col] != 0) {
			continue;
		}
		numOfPsbVals = possiableValuesforCell((*PPGame)->gameBoard,
				(*PPGame)->m, (*PPGame)->n, row, col, &lstOfPsbVal);
		if (numOfPsbVals == 1) {/*auto filling cells and adding change to a move*/
			autofillGameBoard[row * (*PPGame)->N + col] = lstOfPsbVal[0];
			numOfMoves++;
			moves = realloc(moves, sizeof(Move*) * numOfMoves);
			newMove(&(moves[numOfMoves - 1]), row, col, 0, lstOfPsbVal[0]);
			printf("cell %d,%d changed to %d\n", col + 1, row + 1,
					lstOfPsbVal[0]);
		}
		if (numOfPsbVals > 0) {
			free(lstOfPsbVal);
		}
	}
}
free((*PPGame)->gameBoard);
(*PPGame)->gameBoard = autofillGameBoard;
updateErroneuos((*PPGame), -1, -1, erroneusBoardMoves,
		PnumOfErroneusBoardChanges);
addNodeFromCurr(&((*PPGame)->moveList), moves, numOfMoves, *erroneusBoardMoves,
		**PnumOfErroneusBoardChanges);/*adding node to undo/redo list*/
free(*PnumOfErroneusBoardChanges);
free(PnumOfErroneusBoardChanges);
free(erroneusBoardMoves);
printGameBoard((*PPGame), *gameMode, *markErrors);
isWinner(PPGame, gameMode);
}
/*undo all moves, reverting the board to its original loaded state, while traveling backward on the und/redo list
 * and undoing all changes to game board and erroneous board.*/
void reset(Game **PPGame, int *gameMode, int *markErrors) {
int move;
while (backCurrPtr(&((*PPGame)->moveList)) == 1) {
	for (move = 0;
			move < (*PPGame)->moveList.current->next->numOfGameBoardMoves;
			move++) {/*Reverting all commend for game board*/
		(*PPGame)->gameBoard[(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->x
				* (*PPGame)->N
				+ (*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->y] =
				(*PPGame)->moveList.current->next->ptrListOfGameBoardMoves[move]->old;
	}
	for (move = 0;
			move < (*PPGame)->moveList.current->next->numOfErrorBoardMoves;
			move++) {/*Reverting all commend for erroneous board*/
		(*PPGame)->erroneousValues[(*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->x
				* (*PPGame)->N
				+ (*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->y] =
				(*PPGame)->moveList.current->next->ptrListOfErrorBoardMoves[move]->old;
	}
}
printGameBoard((*PPGame), *gameMode, *markErrors);
}
