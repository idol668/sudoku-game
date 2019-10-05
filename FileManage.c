/*File Manage module - responsible for dealing with saving and loading files
 * - checking the board valid for saving in edit mode
 * - checking the file contain legal values before loading
 */
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "MainAux.h"
#include "ParserAux.h"
#include "ErrorHandler.h"
#include "LP.h"

/*This func saves the game to a file,if the game is saved successfully 1
 * is returned and the file will contain m and n sizes on the first row,
 *and on the next rows the cells will be printed with spaces and '.' after fixed cells.
 *  other wise 0 returned and the file will not be saved. */
int saveToFile(Game *game, char *filePath, int gameMode) {
	int row, col;
	int checkResult = 2;
	FILE *fp = fopen(filePath, "w");
	if (fp == NULL) {
		printError(FileSaveFailure);
		return 0;
	} else if ((gameMode) == 1 && isErroneous(game) == 1) {
		printError(ErroneousBoard);
		return 0;
	} else if ((isComplete(game) == 1) && (correctSolution(game) == 1)) {
	} else if ((gameMode) == 1
			&& (checkResult = solve(game, NULL, 2, 0, 0, 0, 0)) != 1) {
		printError(saveFailed);
		return 0;
	} else if (gameMode == 1 && checkResult == -1) {
		printError(FileSaveFailure);
		return 0;
	}
	fprintf(fp, "%d %d\n", game->m, game->n);
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			fprintf(fp, "%d", game->gameBoard[row * game->N + col]);
			if ((gameMode == 1 || game->fixedBoard[row * game->N + col] == 1)
					&& (game->gameBoard[row * game->N + col] != 0)) {
				fprintf(fp, ".");
			}
			if (col < game->N - 1) {
				fprintf(fp, " ");
			}
		}
		fprintf(fp, "\n");
	}
	if (fclose(fp) == EOF) {
		printError(FileCloseFailure);
		return 0;
	}
	printf("The File saved successfully\n");
	return 1;
}
/*This func tries to load a game board from file while checking the file for the correct format, if the load was successful the func returns 1
 * and *PPGame points to the new game board and fixed board,and erroneous board that checked for conflicted fix values using erroneousFixCheck.
 * otherwise  is returned.*/
int loadGame(Game **PPGame, char *filePath, int newGameMode) {
	int i, m, n, val;
	char c;
	Game *attemptRead = NULL;
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL) {
		printError(FileOpenFailure);
		return 0;
	} else if (fscanf(fp, "%d", &m) != 1) {
		printError(FileOpenFailure);
		printDetails(InvalidFile);
		return 0;
	} else if (fscanf(fp, "%d", &n) != 1) {
		printError(FileOpenFailure);
		printDetails(InvalidFile);
		return 0;
	} else if (m * n > 99) {
		printError(limitedRange);
		return 0;

	} else if (m < 0 || n < 0) {
		printError(positiveValue);
		return 0;
	}

	newGame(&attemptRead, m, n);
	for (i = 0; i < attemptRead->N * attemptRead->N; i++) {
		if (fscanf(fp, "%d", &val) == 1) {
			if (inRange(val, 0, m * n)) {
				attemptRead->gameBoard[i] = val;
			} else {
				printError(FileNotInRange);
				freeGame(attemptRead);
				return 0;
			}
			if ((c = fgetc(fp)) == '.') {/*checking if fixed cell*/
				if (val == 0) {
					printError(EmptyFix);
					freeGame(attemptRead);
					return 0;
				} else if (newGameMode == 2) {
					attemptRead->fixedBoard[i] = 1;
				}
				c = fgetc(fp);
				if (c != '\n' && c != '\t' && c != ' ' && c != EOF
						&& c != '\r') {
					printError(FileOpenFailure);
					printDetails(InvalidFile);
					freeGame(attemptRead);
					return 0;
				}

			} else if (c != '\n' && c != '\t' && c != ' ' && c != EOF
					&& c != '\r') {/*skip spaces*/
				printError(FileOpenFailure);
				printDetails(InvalidFile);
				freeGame(attemptRead);
				return 0;
			}
		} else {
			printError(FileOpenFailure);
			printDetails(InvalidFile);
			freeGame(attemptRead);
			return 0;
		}
	}
	while ((c = getc(fp)) == ' ' || c == '\n' || c == '\t' || c == '\r')
		;/*skip spaces*/
	if (!feof(fp)) {/*excess chars in file*/
		printError(ManyChar);
		freeGame(attemptRead);
		return 0;
	} else if (erroneousFixCheck(attemptRead)) {/*check conflicting fix cells using erroneousFixCheck */
		printError(ErroneousFix);
		freeGame(attemptRead);
		return 0;
	}
	freeGame(*PPGame);
	(*PPGame) = attemptRead;
	if (fclose(fp) == EOF) {
		printError(FileCloseFailure);
		return 0;
	}
	return 1;
}

