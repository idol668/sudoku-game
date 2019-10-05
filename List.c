/*List module - This module implements a linked list data structure for redo and undo commands. */
#include <stdio.h>
#include <stdlib.h>
#include "List.h"
/*initiate a new list,assert head and current node pointers point to head node.*/
void newList(List * list) {
	list->ptrHead = &(list->anchorHead);
	list->current = &(list->anchorHead);
	list->anchorHead.next = NULL;
	list->anchorHead.prev = NULL;
}
/*create a new move for a node in the undo/redo list. */
void newMove(Move ** move, int x, int y, int old, int new) {
	(*move) = calloc(1, sizeof(Move));
	(*move)->x = x;
	(*move)->y = y;
	(*move)->old = old;
	(*move)->new = new;
}
/*free memory of a node, including his list of moves in both game board and erroneous board.*/
void freeNode(Node * node) {
	int i;
	for (i = 0; i < node->numOfGameBoardMoves; i++) {
		free(node->ptrListOfGameBoardMoves[i]);
	}
	if (node->numOfGameBoardMoves > 0) {
		free(node->ptrListOfGameBoardMoves);
	}
	for (i = 0; i < node->numOfErrorBoardMoves; i++) {
		free(node->ptrListOfErrorBoardMoves[i]);
	}
	if (node->numOfErrorBoardMoves > 0) {
		free(node->ptrListOfErrorBoardMoves);
	}
	free(node);
}
/*deletes all the nodes in a list from node func argument and forward.*/
void clearList(Node * node) {
	Node * temp;
	int firstDel = 0;
	while (node->next != NULL) {
		if (firstDel == 0) {
			firstDel = 1;
			node->prev->next = NULL;
		}
		temp = node->next;
		freeNode(node);
		node = temp;
	}
	if (firstDel == 0) {
		node->prev->next = NULL;
	}
	freeNode(node);
}
/*delete all nodes from the current node and forward using clearList, and adding a node to the end of the list
 * with moves lists of game board and erroneous board.*/
void addNodeFromCurr(List * list, Move ** ptrListOfGameBoardMoves, int numOfGameBoardMoves,
		Move ** ptrListOfFixedBoardMoves, int numOfFfixedBoardMoves) {
	Node * node = calloc(1, sizeof(Node));
	node->ptrListOfGameBoardMoves = ptrListOfGameBoardMoves;
	node->numOfGameBoardMoves = numOfGameBoardMoves;
	node->ptrListOfErrorBoardMoves = ptrListOfFixedBoardMoves;
	node->numOfErrorBoardMoves = numOfFfixedBoardMoves;
	node->next = NULL;
	if (list->current->next != NULL) {
		clearList(list->current->next);
	}
	list->current->next = node;
	node->prev = list->current;
	list->current = list->current->next;
}
/*moving current node pointer to the next node. return 1 if the pointer changed ,
 * or 0 if there is no next node to move to and the pointer hasn't changed  .*/
int forwardCurrPtr(List * list) {
	if (list->current->next == NULL) {
		return 0;
	} else {
		list->current = list->current->next;
		return 1;
	}

}
/*Return 1 if the list is empty, and 0 otherwise.*/
int isListEmpty(List * list) {
	if (list->anchorHead.next == NULL) {
		return 1;
	}
	return 0;
}
/*moving current node pointer to the previous node. return 1 if the pointer changed ,
 * or 0 if there is no previous node to move to and the pointer hasn't changed  .*/
int backCurrPtr(List * list) {
	if (isListEmpty(list) || list->current == list->ptrHead) {
		return 0;
	} else {
		list->current = list->current->prev;
		return 1;
	}
}
/*delete the entire list using clearList.*/
void freeList(List * list) {
	if (!isListEmpty(list)) {
		clearList(list->anchorHead.next);
	}
}

