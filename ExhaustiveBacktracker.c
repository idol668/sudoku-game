/*ExhaustiveBacktracker module - In this model, there is one function that uses the exhaustive
 * backtracking algorithm to find the number of possible solutions for the Sudoku game board while using
 * a the stack structure module for implementing the algorithm. */

#include "MainAux.h"
#include "Game.h"
#include "Stack.h"
#include <stdlib.h>
#include <stdio.h>
/* This func finds the number of possible solutions to the input game.
 * In the func there is a use of a custom stack,where each node is a game board state.
 * An element(node) is added(push) to the stack for checking a new possible partial solution (value added to next available cell),
 * and pop when current options for solution are exhausted.
 *
 * */
int numOfSolutions(Game * game) {
	int res = 0, startRow = 0, startCol = 0;
	int * startBoard;
	stack stk;/*initiate stack and first element*/
	elem startElem;
	stk.top = &startElem;
	startBoard = copyBoard(game->gameBoard, game->N * game->N);
	startElem.boardGame = startBoard;
	startElem.next = NULL;
	while (startBoard[startRow * game->N + startCol] != 0) {/*finds first empty cell*/
		startCol++;
		if (startCol == game->N) {
			if (startRow == game->N - 1) {
				return 0;
			}
			startCol = 0;
			startRow++;
		}
	}
	startElem.col = startCol;
	startElem.row = startRow;
	startElem.possiableValueIndex = 0;
	startElem.initFlag = 0;
	startElem.nextCol = startCol;
	startElem.nextRow = startRow;

	while (!isEmpty(&stk)) {
		if (stk.top->initFlag == 0) {/*initiate element's possible values list for cell and finding next empty cell */
			stk.top->numOfPossiableValues = possiableValuesforCell(stk.top->boardGame, game->m,
					game->n, stk.top->row, stk.top->col, &(stk.top->possiableValues));
			do {
				stk.top->nextCol++;
				if (stk.top->nextCol == game->N) {
					if (stk.top->nextRow == game->N - 1) {
						break;
					}
					stk.top->nextCol = 0;
					stk.top->nextRow++;
				}
			} while (stk.top->boardGame[stk.top->nextRow * game->N + stk.top->nextCol] != 0);
			stk.top->initFlag = 1;
		}
		if (stk.top->nextCol == game->N) {/*solution board found*/
			res += stk.top->numOfPossiableValues;
			pop(&stk);
			continue;
		}
		if (stk.top->possiableValueIndex < stk.top->numOfPossiableValues) {/*trying all cell possible values*/
			int * newBoardGame;
			stk.top->boardGame[stk.top->row * game->N + stk.top->col] =
					stk.top->possiableValues[stk.top->possiableValueIndex];
			stk.top->possiableValueIndex++;
			newBoardGame = copyBoard(stk.top->boardGame, game->N * game->N);
			push(newBoardGame, stk.top->nextRow, stk.top->nextCol, &stk);
			continue;
		}
		pop(&stk);
	}
	return res;
}
