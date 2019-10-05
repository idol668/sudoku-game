/*
 * LP.c -  This module in charge of solving the game board using integer linear programming for validating board and giving hints,
 * or guessing a solution to the board using linear programming. The main function in the module is solve which uses the gurobi optimizer for adding constrains and objective function
 *  suitable for the mode required of solving the board.
 */

#include "LP.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gurobi_c.h"
#include "Game.h"
#include "ErrorHandler.h"
/*
 *return the index of value in array if value is indeed in array or -1 if value not in array.*/
int valueInArray(int value, int * array, int arraySize) {
	int i;
	if (array != NULL) {
		for (i = 0; i < arraySize; i++) {
			if (array[i] == value) {
				return i;
			}
		}
	}
	return -1;
}
/*free all allocated memory for solve function*/
void freeLPMem(cellOfvalues * possiableValuesBoard, int boardSize, double * sol,
		int * ind, double * val, double * obj, char * vtype, GRBenv *env,
		GRBmodel *model) {
	int i;
	for (i = 1; i < boardSize + 1; i++) {
		if (possiableValuesBoard[i].possiableValues != NULL) {
			free(possiableValuesBoard[i].possiableValues);
		}
	}
	free(possiableValuesBoard);
	free(sol);
	free(ind);
	free(val);
	free(obj);
	free(vtype);
	if (model != NULL) {
		GRBfreemodel(model);
	}
	if (env != NULL) {
		GRBfreeenv(env);
	}
}
/*initiate an array of cellOfVales struct for computing the number of possible values for each cell in game board(using possiableValuesforCell),and the accumulative number of possible values for all the cell.
 * each possible values for a cell is an variable in the linear programming algorthm.for example if the possible values the first two cell are 1,5 and 2,4 respectively ,
 * then the first variable will represent if there is 1 in the first cell or not(for ILP) and the third variable will represent if there is 2 in the second cell or not(for ILP).
 * filled cells will have 0 variables.  */
void initiatePossibleValuesBoard(Game* game, cellOfvalues * posblValsBoard) {
	int index;
	posblValsBoard[0].accumPossVals = 0;/*first cell is empty for convenience of code.*/
	posblValsBoard[0].numOfpossVals = 0;
	posblValsBoard[0].possiableValues = NULL;
	for (index = 1; index < (game->N * game->N) + 1; index++) {
		if (game->gameBoard[index - 1] == 0) {/*filling list and amount of possible values for empty cell.*/
			int row = (index - 1) / game->N;
			int col = (index - 1) % game->N;
			posblValsBoard[index].numOfpossVals = possiableValuesforCell(
					game->gameBoard, game->m, game->n, row, col,
					&(posblValsBoard[index].possiableValues));
			posblValsBoard[index].accumPossVals =
					posblValsBoard[index].numOfpossVals
							+ posblValsBoard[index - 1].accumPossVals;
		} else {/*filled cell in game board.*/
			posblValsBoard[index].numOfpossVals = 0;
			posblValsBoard[index].accumPossVals =
					(posblValsBoard[index - 1].accumPossVals);
		}
	}
}
/*set an objective function to maximize ,for ILP just one random variable with coefficient of 1 and for LP the coefficient of the variables
 *is (1/number of possible values of the cell the variable is related to). This objective function gives an advantage for setting a value to a cell with small 
   *amount possible values than setting  the value in a neighbor cell  with more possible values. also setting the type of the variables for ILP/LP.*/
void setVerTypeAndObjFunc(Game* game, cellOfvalues * posblValsBoard,
		char * vtype, double * obj, int mode) {
	int index, i;
	if (mode == 0 || mode == 1) {/*for linear programming */
		for (i = 0; i < posblValsBoard[game->N * game->N].accumPossVals; i++) {/*continues variables*/
			vtype[i] = GRB_CONTINUOUS;
		}
		for (i = 1; i < game->N * game->N + 1; i++) {
			if (posblValsBoard[i].numOfpossVals > 0) {
				for (index = 0; index < posblValsBoard[i].numOfpossVals;
						index++) {/*described in func comment */
					obj[posblValsBoard[i - 1].accumPossVals + index] = (1.0
							/ posblValsBoard[i].numOfpossVals);
				}
			}
		}
	} else {/*for integer linear programming */
		for (i = 0; i < posblValsBoard[game->N * game->N].accumPossVals; i++) {/*binary variables*/
			vtype[i] = GRB_BINARY;
		}
		obj[rand() % (posblValsBoard[game->N * game->N ].accumPossVals)] = 1;/*maximize random variable*/
	}
}
/*set constrain for each non empty cell must have a single value.*/
int setCellsConstraints(Game* game, cellOfvalues * posblValsBoard,
		GRBmodel *model, double * sol, double * val, int * ind, double * obj,
		char * vtype, GRBenv *env) {
	int cellIdx, i, error;
	for (cellIdx = 1; cellIdx < game->N * game->N + 1; cellIdx++) {
		if (posblValsBoard[cellIdx].numOfpossVals == 0
				&& game->gameBoard[cellIdx - 1] == 0) {/*empty cell with no possible values.*/
			error = GRBaddconstr(model, 0, ind, val, GRB_EQUAL, 1, NULL);
			if (error) {
				freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj,
						vtype, env, model);
				return 0;
			}
		} else if (posblValsBoard[cellIdx].numOfpossVals > 0) {/*empty cell with possible values.*/
			for (i = 0; i < posblValsBoard[cellIdx].numOfpossVals; i++) {
				ind[i] = posblValsBoard[cellIdx - 1].accumPossVals + i;
				val[i] = 1;
			}
			error = GRBaddconstr(model, posblValsBoard[cellIdx].numOfpossVals,
					ind, val, GRB_EQUAL, 1, NULL);
			if (error) {
				freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj,
						vtype, env, model);
				return 0;
			}
		}
	}
	return 1;
}
/*set constrain such that each value (that hasn't appear yet) must appear once in each row.using possiableValuesforColOrRow for finding row possible values.*/
int setRowsConstraints(Game* game, cellOfvalues * posblValsBoard,
		GRBmodel *model, double * sol, double * val, int * ind, double * obj,
		char * vtype, GRBenv *env) {
	int row, numOfRowPsbleVal, value, index, cellInRow, idxOfCellPsbleVal,
			*possiableValues, error;
	for (row = 0; row < game->N; row++) {
		numOfRowPsbleVal = possiableValuesforColOrRow(game->gameBoard, game->m,
				game->n, row, &possiableValues, rows);
		if (numOfRowPsbleVal > 0) {
			for (value = 1; value <= game->N; value++) {
				if (valueInArray(value, possiableValues, numOfRowPsbleVal)
						> -1) {/*check if value missing from row*/
					index = 0;
					for (cellInRow = 1; cellInRow < game->N + 1; cellInRow++) {
						if ((idxOfCellPsbleVal =
								valueInArray(value,
										posblValsBoard[row * game->N + cellInRow].possiableValues,
										posblValsBoard[row * game->N + cellInRow].numOfpossVals))
								> -1) {/*check if the value can appear in that cell*/
							ind[index] = posblValsBoard[row * game->N
									+ cellInRow - 1].accumPossVals
									+ idxOfCellPsbleVal;
							val[index] = 1;
							index++;
						}
					}
					if (index > 0) {/*add constarin*/
						error = GRBaddconstr(model, index, ind, val, GRB_EQUAL,
								1,
								NULL);
						if (error) {
							freeLPMem(posblValsBoard, game->N * game->N, sol,
									ind, val, obj, vtype, env, model);
							return 0;
						}
					}
				}
			}
			free(possiableValues);
		}
	}
	return 1;
}
/*set constrain such that each value (that hasn't appear yet) must appear once in each column.using possiableValuesforColOrRow for finding column possible values.*/
int setColsConstraints(Game* game, cellOfvalues * posblValsBoard,
		GRBmodel *model, double * sol, double * val, int * ind, double * obj,
		char * vtype, GRBenv *env) {
	int col, cellInCol, numOfColPsbleVal, *possiableValues, value, index,
			idxOfCellPsbleVal, error;
	for (col = 0; col < game->N; col++) {
		numOfColPsbleVal = possiableValuesforColOrRow(game->gameBoard, game->m,
				game->n, col, &possiableValues, column);
		if (numOfColPsbleVal > 0) {
			for (value = 1; value <= game->N; value++) {
				if (valueInArray(value, possiableValues, numOfColPsbleVal)
						> -1) {/*check if value missing from column*/
					index = 0;
					for (cellInCol = 0; cellInCol < game->N; cellInCol++) {
						if ((idxOfCellPsbleVal =
								valueInArray(value,
										posblValsBoard[col + 1
												+ cellInCol * game->N].possiableValues,
										posblValsBoard[col + 1
												+ cellInCol * game->N].numOfpossVals))
								> -1) {/*check if the value can appear in that cell*/
							ind[index] = posblValsBoard[col + 1
									+ cellInCol * game->N - 1].accumPossVals
									+ idxOfCellPsbleVal;
							val[index] = 1;
							index++;
						}
					}
					if (index > 0) {
						error = GRBaddconstr(model, index, ind, val, GRB_EQUAL,
								1,
								NULL);
						if (error) {
							freeLPMem(posblValsBoard, game->N * game->N, sol,
									ind, val, obj, vtype, env, model);
							return 0;
						}
					}
				}
			}
			free(possiableValues);
		}
	}
	return 1;
}
/*set constrain such that each value (that hasn't appear yet) must appear once in each block .using possiableValuesforBlock for finding block possible values.*/
int setBlksConstraints(Game* game, cellOfvalues * posblValsBoard,
		GRBmodel *model, double * sol, double * val, int * ind, double * obj,
		char * vtype, GRBenv *env) {
	int block, numOfBlkPsbleVal, *possiableValues, value, index, rowInBlk,
			colInBlk, idxOfCellPsbleVal, error;
	for (block = 0; block < game->N; block++) {
		numOfBlkPsbleVal = possiableValuesforBlock(game->gameBoard, game->m,
				game->n, block, &possiableValues);
		if (numOfBlkPsbleVal > 0) {
			for (value = 1; value <= game->N; value++) {
				if (valueInArray(value, possiableValues, numOfBlkPsbleVal)
						> -1) {
					index = 0;
					for (rowInBlk = 0; rowInBlk < game->m; rowInBlk++) {
						for (colInBlk = 0; colInBlk < game->n; colInBlk++) {
							if ((idxOfCellPsbleVal = valueInArray(value,
									posblValsBoard[((block / game->m) * game->m
											+ rowInBlk) * game->N
											+ (block % game->m) * game->n
											+ colInBlk + 1].possiableValues,
									posblValsBoard[((block / game->m) * game->m
											+ rowInBlk) * game->N
											+ (block % game->m) * game->n
											+ colInBlk + 1].numOfpossVals))
									> -1) {
								ind[index] =
										posblValsBoard[((block / game->m)
												* game->m + rowInBlk) * game->N
												+ (block % game->m) * game->n
												+ colInBlk].accumPossVals
												+ idxOfCellPsbleVal;
								val[index] = 1;
								index++;
							}
						}
					}
					if (index > 0) {
						error = GRBaddconstr(model, index, ind, val, GRB_EQUAL,
								1, NULL);
						if (error) {
							freeLPMem(posblValsBoard, game->N * game->N, sol,
									ind, val, obj, vtype, env, model);
							return 0;
						}
					}
				}
			}
			free(possiableValues);
		}
	}
	return 1;
}
/* In linear programming ,set constrain for each variable x ,such that x=<1 nd 0=<x.*/
int setRangeConstraints(Game* game, cellOfvalues * posblValsBoard,
		GRBmodel *model, double * sol, double * val, int * ind, double * obj,
		char * vtype, GRBenv *env) {
	int i, error, secError;
	for (i = 0; i < posblValsBoard[game->N * game->N].accumPossVals; i++) {
		ind[0] = i;
		val[0] = 1;
		error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0,
		NULL);/*0=<x*/
		secError = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1.0,
		NULL);/*x=<1*/
		if (error || secError) {
			freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj,
					vtype, env, model);
			return 0;
		}
	}
	return 1;
}
/*print the scores for cell guessRow,guessCol the are greater than zero.*/
void printGuessHint(Game* game, cellOfvalues * posblValsBoard, int guessRow,
		int guessCol, double * sol) {
	int index = 0, i;
	printf("Possible values for cell %d,%d are:\n", guessCol + 1, guessRow + 1);
	for (i = 0;
			i < posblValsBoard[guessRow * game->N + guessCol + 1].numOfpossVals;
			i++) {
		if (sol[posblValsBoard[guessRow * game->N + guessCol].accumPossVals + i]
				> 0) {
			index++;
			printf("The %d value number is %d with a score of %f\n", index,
					posblValsBoard[guessRow * game->N + guessCol + 1].possiableValues[i],
					sol[posblValsBoard[guessRow * game->N + guessCol].accumPossVals
							+ i]);
		}
	}
}
/*Fills *ptrSolvedBoard with guess values with scores greater than threshold(random one if several exist.  */
void fillBoardForGuess(Game* game, cellOfvalues * posblValsBoard,
		int ** ptrSolvedBoard, float threshold, double * sol) {
	int index, row, col, indexInPossBoard, newCellNumOfposbVal,
			numOfPassedthresh, onlyVal, value;
	float sumOfChanes, chance, accumulativeChance;
	int * newPossVals = NULL;
	(*ptrSolvedBoard) = copyBoard(game->gameBoard, game->N * game->N);
	for (index = 0; index < game->N * game->N; index++) {
		if (game->gameBoard[index] == 0) {/*for each empty cell*/
			row = index / game->N;
			col = index % game->N;
			newCellNumOfposbVal = possiableValuesforCell((*ptrSolvedBoard),
					game->m, game->n, row, col, &newPossVals);/*check possible values for cell after previous cell were already filled.*/
			numOfPassedthresh = 0;
			sumOfChanes = 0;
			for (value = 0; value < newCellNumOfposbVal; value++) {
				indexInPossBoard = valueInArray(newPossVals[value],
						posblValsBoard[index + 1].possiableValues,
						posblValsBoard[index + 1].numOfpossVals);
				if (sol[posblValsBoard[index].accumPossVals + indexInPossBoard]
						>= threshold) {/*if value grater than threshold,count it and add it's score to sumOfChanes */
					sumOfChanes += sol[posblValsBoard[index].accumPossVals
							+ indexInPossBoard];
					numOfPassedthresh++;
					onlyVal = value;/*in case of only one value surpass threshold.*/
				}
			}
			if (numOfPassedthresh == 0) {/*no value is suitable*/
				if (newCellNumOfposbVal > 0) {
					free(newPossVals);
				}
				continue;
			} else if (numOfPassedthresh == 1) {/*one value is suitable*/
				(*ptrSolvedBoard)[index] = newPossVals[onlyVal];
			} else {/*random raffle will occur to pick a value with  chances respectively to scores*/
				chance = (float) rand() / RAND_MAX;
				accumulativeChance = 0;
				for (value = 0; value < numOfPassedthresh; value++) {
					indexInPossBoard = valueInArray(newPossVals[value],
							posblValsBoard[index + 1].possiableValues,
							posblValsBoard[index + 1].numOfpossVals);
					if (sol[posblValsBoard[index].accumPossVals
							+ indexInPossBoard] >= threshold) {
						accumulativeChance += newPossVals[value];
						if (chance < (accumulativeChance / sumOfChanes)) {
							(*ptrSolvedBoard)[index] = newPossVals[value];
							break;
						}
					}
				}
			}
			if (newCellNumOfposbVal > 0) {
				free(newPossVals);
			}
		}
	}
}
/*Fills *ptrSolvedBoard according to the solution found to the variables. */
void fillSolveBoard(Game* game, cellOfvalues * posblValsBoard,
		int ** ptrSolvedBoard, double * sol) {
	int row, col, value;
	(*ptrSolvedBoard) = copyBoard(game->gameBoard, game->N * game->N);
	for (row = 0; row < game->N; row++) {
		for (col = 0; col < game->N; col++) {
			if (game->gameBoard[row * game->N + col] == 0) {
				for (value = 0;
						value
								< posblValsBoard[row * game->N + col + 1].numOfpossVals;
						value++) {
					if (sol[posblValsBoard[row * game->N + col].accumPossVals
							+ value] == 1) {
						(*ptrSolvedBoard)[row * game->N + col] =
								posblValsBoard[row * game->N + col + 1].possiableValues[value];
					}
				}
			}
		}
	}
}
/*This func solve the game board using gurobi optimizer which uses linear programming or integer linear programming depending on the input mode.
 In all modes the func create a variable for each possible value of all the empty cells in the board and add constraints for cell nihborhood validity.
 In mode 0 which is Guess hint mode ,the func add range constrains for every variable to be continuous and between 0 and 1 ,the objective function explained above,
 eventually the possible values with score greater than zero of cell in position guessRow,guessCol will be printed.
 In mode 1 which is Guess mode  the func add range constrains for every variable to be continuous and between 0 and 1 ,the objective function explained above,
 eventually cells who has possible values with score greater than threshold will be fill into *ptrSolvedBoard in random manner with respect to their scores.
 In mode 2 which is for solving the board ,every variable is binary ,the variable that set to one after solving inform what the solution value is for each empty
 cell, if returnResult ==1 then the solution is set to *ptrSolvedBoard.
 The function returns 1 if solution found, 0 if no solution found and -1 in case f gurobi related error.
 */
int solve(Game* game, int ** ptrSolvedBoard, int mode, float threshold,
		int guessRow, int guessCol, int returnResult) {
	double * sol;
	double * val;
	int * ind;
	double * obj;
	int optimstatus;
	/*double objval;*/
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	char * vtype;
	int error;
	cellOfvalues * posblValsBoard = calloc((game->N * game->N + 1),
			sizeof(cellOfvalues));
	initiatePossibleValuesBoard(game, posblValsBoard);
	sol = calloc(posblValsBoard[game->N * game->N].accumPossVals,
			sizeof(double));
	ind = calloc(game->N, sizeof(int));
	val = calloc(game->N, sizeof(double));
	obj = calloc(posblValsBoard[game->N * game->N].accumPossVals,
			sizeof(double));
	vtype = calloc(posblValsBoard[game->N * game->N].accumPossVals,
			sizeof(char));
	if (posblValsBoard == NULL || sol == NULL || ind == NULL || val == NULL
			|| obj == NULL || vtype == NULL) {
		printError(MemoryFailed);
		printDetails(exiting);
		exit(0);
	}
	srand(time(0));
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL,
	NULL);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/*set an objective function to maximize ,for ILP just one random variable with coefficient of 1 and for LP the coefficient of the variables
	 *  is (1/number of possible values of the cell the variable is related to).also setting the type of the variables for ILP/LP.*/
	setVerTypeAndObjFunc(game, posblValsBoard, vtype, obj, mode);
	/* add variables to model */
	error = GRBaddvars(model, posblValsBoard[game->N * game->N].accumPossVals,
			0,
			NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/*each cell have a value*/
	if (setCellsConstraints(game, posblValsBoard, model, sol, val, ind, obj,
			vtype, env) == 0) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/*each value must appears once in each row*/
	if (setRowsConstraints(game, posblValsBoard, model, sol, val, ind, obj,
			vtype, env) == 0) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/*each value must appear once in each column*/
	if (setColsConstraints(game, posblValsBoard, model, sol, val, ind, obj,
			vtype, env) == 0) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/*each value must appear once in each block*/
	if (setBlksConstraints(game, posblValsBoard, model, sol, val, ind, obj,
			vtype, env) == 0) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	if (mode == 0 || mode == 1) {
		/* Constrain variables to the desired range */
		if (setRangeConstraints(game, posblValsBoard, model, sol, val, ind, obj,
				vtype, env) == 0) {
			freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj,
					vtype, env, model);
			return -1;
		}
	}
	/* Optimize model  */
	error = GRBoptimize(model);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/* Write model to 'mip1.lp' */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}
	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return -1;
	}

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		/* get the solution - the assignment to each variable */
		if (returnResult == 1 || mode == 0 || mode == 1) {
			error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,
					posblValsBoard[game->N * game->N].accumPossVals, sol);
			if (error) {
				freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj,
						vtype, env, model);
				return -1;
			}
		}
		if (mode == 0) {/*guess hint command*/
			printGuessHint(game, posblValsBoard, guessRow, guessCol, sol);
		} else if (mode == 1) {/*Guess command*/
			fillBoardForGuess(game, posblValsBoard, ptrSolvedBoard, threshold,
					sol);
		} else if (mode == 2 && returnResult == 1) {/*Fill the board for hint and generate commends*/
			fillSolveBoard(game, posblValsBoard, ptrSolvedBoard, sol);
		}
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype,
				env, model);
		return 0;
	}
	/* error or calculation stopped */
	else {
		return 0;
	}
	/* Free model and environment */
	freeLPMem(posblValsBoard, game->N * game->N, sol, ind, val, obj, vtype, env,
			model);
	return 1;
}
