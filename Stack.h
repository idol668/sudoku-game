/*
 * This module implement a stack for the backTracer function.
 * It includes support for push, pop and checking if the stack is empty.
 *
 * */
#ifndef STACK_H_
#define STACK_H_
/*element structure ,contains the row and column of the cell currently trying to fill with his possible values,
 * row and column  of the next empty cell , a copy of current board state and pointer to next element in the stack.*/
typedef struct elem{
	int row, col, possiableValueIndex, numOfPossiableValues,nextRow,nextCol,initFlag;
	int* possiableValues;
	int * boardGame;
	struct elem * next;
}elem;

/*stack structure that has a pointer to the top element. */
typedef struct boardStack{
	elem * top;
}stack;

int push(int * Board ,int row, int col,stack * stk);
void pop (stack * stk);
int isEmpty(stack * stk);

#endif /* STACK_H_ */
