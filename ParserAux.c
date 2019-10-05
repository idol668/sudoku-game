/*ParserAux module - valid checks are performed for commends parameters received from the user in the parser model
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "Game.h"
#include "ErrorHandler.h"

/*checks if str is an integer ,if so return the integer,otherwise return -1. if isIndex==1 then integer-1 is returned.*/
int isInteger(char * str, int isIndex) {
	int res;
	int len;
	if (sscanf(str, "%d%n", &res, &len) == 1 && str[len] == '\0') {
		if (isIndex) {
			res--;
		}
	} else {
		return -1;
	}
	return res;
}
/*return 1 if y=<x and x=<z ,and 0 otherwise.*/
int inRange(int x, int y, int z) {
	if (x >= y && x <= z) {
		return 1;
	}
	return 0;
}
/*/*checks if str is an floating point number ,if so return the number,otherwise return -1.*/
float getThreshold(char* str) {
	float res;
	int len;
	if (sscanf(str, "%f%n", &res, &len) == 1 && str[len] == '\0') {
		return res;
	} else {
		return -1;
	}
}
/*checks the if the parameters given are in the right amount.if so return 1 otherwise return -1 and print suitable error message. */
int checkParameters(char* firstArg, char* secondArg, char* thirdArg,
		char* fourthArg, int amount) {
	if (amount == 0) {
		if (firstArg != NULL) {
			printError(NoParams);
			return -1;
		}
	}
	if (amount == 1) {
		if (firstArg == NULL) {
			printError(FewParams);
			printDetails(needoneParameter);
			return -1;
		} else if (secondArg != NULL) {
			printError(ManyParams);
			printDetails(needoneParameter);
			return -1;
		}
	}
	if (amount == 2) {
		if (firstArg == NULL || secondArg == NULL) {
			printError(FewParams);
			printDetails(needtwoParameter);
			return -1;
		} else if (fourthArg != NULL) {
			printError(ManyParams);
			printDetails(needtwoParameter);
			return -1;
		}
	}
	if (amount == 3) {
		if (firstArg == NULL || secondArg == NULL || thirdArg == NULL) {
			printError(FewParams);
			printDetails(needthreeParameter);
			return -1;
		} else if (fourthArg != NULL) {
			printError(ManyParams);
			printDetails(needthreeParameter);
			return -1;
		}

	}
	if (amount == 4) {/*Special case for edit ,can accept up to one parameter.*/
		if (secondArg != NULL) {
			printError(ManyParams);
			printDetails(needoneParameter);
			return -1;
		}
	}
	return 1;
}
/*checks if the game mode is is one of the legal modes,if so then 1 is returned ,otherwise -1 is returned.*/
int checkMode(int* gameMode, int legal1, int legal2) {
	if (*gameMode != legal1 && *gameMode != legal2) {
		printError(notAvailable);
		if (legal1 == 1 && legal2 == 2) {
			printDetails(EditSolve);
			return -1;
		} else if (legal1 == 1) {
			printDetails(Edit);
			return -1;
		} else if (legal1 == 2) {
			printDetails(Solve);
			return -1;
		}
	}
	return 1;

}
/*check if the two arguments are integers and that they are in the correct range, if so returns 1 , otherwise returns -1
 * and Suitable error massage is printed.*/
int checkRange(Game ** PPGame, int* row, int* col, char* firstArg,
		char* secondArg) {
	if ((*row = isInteger(firstArg, 1)) == -1
			|| (!inRange(*row, 0, (*PPGame)->N - 1))) {
		printError(FirstNotInRange);
		printf(" in range from 1 to %d\n", (*PPGame)->N);
		return -1;
	} else if ((*col = isInteger(secondArg, 1)) == -1
			|| (!inRange(*col, 0, (*PPGame)->N - 1))) {
		printError(SecondNotInRange);
		printf(" in range from 1 to %d\n", (*PPGame)->N);
		return -1;
	}
	return 1;
}
/*check if the cellsToAdd and cellsToKeep are integers and that they are in the correct range, if so returns 1 , otherwise returns -1
 and Suitable error massage is printed.*/
int checkForGenerate(Game** PPGame, int* cellsToAdd, int* cellsToKeep,
		char* firstArg, char* secondArg, int* emptyCells) {
	if (((*cellsToAdd = isInteger(firstArg, 0)) == -1)
			|| !inRange(*cellsToAdd, 0,
					(*emptyCells = numOfEmptyCells(*PPGame)))) {
		printError(FirstNotInRange);
		printf(" in range from 1 to %d\n", *emptyCells);
		return -1;
	} else if (((*cellsToKeep = isInteger(secondArg, 0)) == -1)
			|| !inRange(*cellsToKeep, 0, (*PPGame)->N * (*PPGame)->N)) {
		printError(SecondNotInRange);
		printf(" in range from 0 to %d\n", (*PPGame)->N * (*PPGame)->N);
		return -1;
	} else if (isErroneous((*PPGame))) {
		printError(ErroneousBoard);
		return -1;
	}
	return 1;
}
