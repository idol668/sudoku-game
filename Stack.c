/*
 * This module implement a stack for the backTracer function.
 * It includes support for push, pop and checking if the stack is empty.
 *
 * */

#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "ErrorHandler.h"
/*This func add an element to the stack ,after initialize the element parameters that that was received in func args.
 * */
int push(int * Board, int row, int col, stack * stk) {
	elem * p = calloc(1, sizeof(elem));
	if (p == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
		return 0;
	}
	p->initFlag = 0;
	p->boardGame = Board;
	p->row = row;
	p->col = col;
	p->nextRow = row;
	p->nextCol = col;
	p->possiableValueIndex = 0;
	p->next = stk->top;
	stk->top = p;
	return 1;
}
/*This func delete the top element of the stack and change top to next element or null if last element deleted.
 * */
void pop(stack * stk) {
	elem * p;
	free(stk->top->boardGame);
	free(stk->top->possiableValues);
	p = stk->top;
	if (stk->top->next == NULL) {
		stk->top = NULL;
	} else {
		stk->top = stk->top->next;
		free(p);
	}
}
/*This func returns 1 if the stack is empty and 0 otherwise.
 * */
int isEmpty(stack * stk) {
	if (stk->top != NULL) {
		return 0;
	}
	return 1;
}
