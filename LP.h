

#ifndef LP_H_
#define LP_H_
#include "Game.h"

typedef struct listOfValuesCell {
	int  accumPossVals ,numOfpossVals;
	int * possiableValues;
}cellOfvalues;

int solve(Game* game, int ** ptrSolvedBoard, int mode, float threshold, int guessRow, int guessCol,
		int returnResult);
int valueInArray(int value, int * array, int arraySize);

#endif /* LP_H_ */
