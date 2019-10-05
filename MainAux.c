/*MainAux module - This model is designed to print the game board as well as a game opening message. */
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

/* prints initial game message*/
void initialMessage() {
	printf("Welcome to the sudoku game!\n");
}
/*prints game board to console. fixed cells followed by '.' and erroneous cells  by '*' according to mark_valuse laws.*/
void printGameBoard(Game* game, int gameMode, int markValues) {
	int i, j, p, t, k;
	for (k = 0; k < 4 * (game->N) + game->m + 1; k++) {
		printf("-");
	}
	printf("\n");
	for (i = 0; i < game->n; i++) {
		for (j = 0; j < game->m; j++) {
			printf("|");
			for (p = 0; p < game->m; p++) {
				for (t = 0; t < game->n; t++) {
					if (game->gameBoard[(i * game->m + j) * game->N + p * game->n + t] == 0) {
						printf("    ");
					} else {
						printf(" %2d",
								game->gameBoard[(i * game->m + j) * game->N + p * game->n + t]);
						if (game->fixedBoard[(i * game->m + j) * game->N + p * game->n + t] == 1) {
							printf(".");
						} else if (game->erroneousValues[(i * game->m + j) * game->N + p * game->n
								+ t] == 1
								&& ((gameMode == 1) || (gameMode == 2 && markValues == 1))) {
							printf("*");
						} else {
							printf(" ");
						}
					}
				}
				printf("|");
			}
			printf("\n");
		}
		for (k = 0; k < 4 * (game->N) + game->m + 1; k++) {
			printf("-");
		}
		printf("\n");
	}
}
