/*List module - This module implements a linked list data structure for redo and undo commands.
 * using 3 structs - "move" to remember changes of the board, "list" and "node" for linked-list */

#ifndef LIST_H_
#define LIST_H_
/*
 *  struct move -
 *  "x" for row and "y" for column - use to remember the coordinates of cell that changed,
 *  old for the previous data , and new for the update data
 */
typedef struct move {
	int x , y, old, new;
} Move;
/* struct node - "next" and "prev" are pointers for previous and next nodes in the list.
 * "ptrListOfGameBoardMoves" and "ptrListOfErrorBoardMoves" are pointers to array pointers for Game and Error board moves.
 * in addition, we remember with "numOfGameBoardMoves" and "numOfErrorBoardMoves" the number of changes that occurred.
 *
 */
typedef struct node {
	struct node * next;
	struct node * prev;
	Move** ptrListOfGameBoardMoves;
	Move ** ptrListOfErrorBoardMoves;
	int numOfGameBoardMoves;
	int numOfErrorBoardMoves;
} Node;
/*list struct - "anchorHead" -first node in the list, dummy node for convenience of code.
 * 				"ptrHead" - points to the first node which is an dummy node.
 * 				"current" - points to the node that represent the last changing board commend.
 * */
typedef struct list {
	Node anchorHead;
	Node* ptrHead;
	Node* current;
} List;
int backCurrPtr(List * list);
int forwardCurrPtr(List * list);
void freeList(List * list);
void addNodeFromCurr(List * list, Move ** ptrListOfGameBoardMoves, int numOfGameBoardMoves,
		Move ** ptrListOfFixedBoardMoves, int numOfFfixedBoardMoves);
void newMove(Move ** move, int x, int y, int old, int new);
void newList(List * list);

#endif /* LIST_H_ */
