/* Parser module - designed for parsing user command and react accordingly, check validity of command
 * and send to execution if it valid for game status,and parameters received using he Execution module when needed.
 */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MainAux.h"
#include "FileManage.h"
#include "ParserAux.h"
#include "Game.h"
#include "LP.h"
#include "ExhaustiveBacktracker.h"
#include "List.h"
#include "ErrorHandler.h"
#include "Execution.h"

#define arraySize 258

void userCommand(Game ** PPGame, int * gameMode, int * markErrors) {
	char commandLine[arraySize] = "";
	printf("Please enter a command:\n");
	if (feof(stdin)) {
		printf("Exiting...\n");
		freeGame(*PPGame);
		exit(1);
	}
	if (fgets(commandLine, 258, stdin) != NULL) {
		char * commandName = strtok(commandLine, " \t\r\n");
		char * firstArg = strtok(NULL, " \t\r\n");
		char * secondArg = strtok(NULL, " \t\r\n");
		char * thirdArg = strtok(NULL, " \t\r\n");
		char * fourthArg = strtok(NULL, " \t\r\n");
		if (commandLine[256] != '\n' && commandLine[256] != 0) {/*check commend length*/
			char c;
			printError(InputTooLong);
			while ((c = getchar()) != '\n' && c != EOF)
				;
		} else if (commandName == NULL) {/*empty of content input*/
		} else if (strcmp(commandName, "solve") == 0) {/* **** Solve command  **** */
			if (checkParameters(firstArg, secondArg, thirdArg, fourthArg, 1)
					== -1) {
			} else {
				solveMode(PPGame, gameMode, markErrors, firstArg);
			}
		} else if (strcmp(commandName, "edit") == 0) {/* **** Edit command **** */
			if (checkParameters(firstArg, secondArg, thirdArg, fourthArg, 4)
					== -1) {
			} else {
				editMode(PPGame, gameMode, markErrors, firstArg);
			}
		} else if ((strcmp(commandName, "mark_errors") == 0)) {/* **** Mark errors command **** */
			if (checkMode((gameMode), 2, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					1) == -1) {
			} else {
				markErrros(markErrors, firstArg);
			}
		} else if (strcmp(commandName, "print_board") == 0) {/* **** Print board command *****/
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else {
				printGameBoard((*PPGame), *gameMode, *markErrors);
			}
		} else if (strcmp(commandName, "set") == 0) {/* **** Set command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					3) == -1) {
			} else {
				set(PPGame, gameMode, markErrors, firstArg, secondArg,
						thirdArg);
			}
		} else if (strcmp(commandName, "validate") == 0) {/* **** Validate command **** */
			int checkResult=2;
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else if (isErroneous(*PPGame)) {
				printError(ErroneousBoard);
			}else if((isComplete(*PPGame)==1)&&(correctSolution(*PPGame)==1)){
				printf("The board is full with a correct solution\n");
			} else if ((checkResult=solve(*PPGame, NULL, 2, 0, 0, 0, 0)) == 1) {
				printf("The board is solvable\n");
			} else if (checkResult==0) {
				printError(UnsolvableBoard);
			} else if (checkResult==-1){
				printError(validateFailed);
			}
		} else if (strcmp(commandName, "guess") == 0) {/* **** Guess command **** */
			float threshold = 0;
			if (checkMode((gameMode), 2, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					1) == -1) {
			} else if ((threshold = getThreshold(firstArg)) == -1 || threshold<0 || threshold>1) {
				printError(firstfloat);
			} else if (isErroneous(*PPGame)) {
				printError(ErroneousBoard);
			} else {
				guess(PPGame, gameMode, markErrors, threshold);
			}
		} else if (strcmp(commandName, "generate") == 0) {/* **** Generate command **** */
			int intX, intY, emptyCells;
			int * tempBoard = NULL;
			int numOfMoves = 0;
			Move ** moves = NULL;
			if (checkMode((gameMode), 1, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					2) == -1) {
			} else if (checkForGenerate(PPGame, &intX, &intY, firstArg,
					secondArg, &emptyCells) == -1) {
			} else {
				generate(PPGame, gameMode, markErrors, intX, intY, emptyCells,
						tempBoard, numOfMoves, moves);
			}
		} else if (strcmp(commandName, "undo") == 0) { /* **** Undo command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else if (backCurrPtr(&((*PPGame)->moveList)) == 0) {
				printError(UndoUnavailable);
			} else {
				undo(PPGame, gameMode, markErrors);
			}
		} else if (strcmp(commandName, "redo") == 0) {/* **** Redo command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else if (forwardCurrPtr(&((*PPGame)->moveList)) == 0) {
				printError(RedoUnavailable);
			} else {
				redo(PPGame, gameMode, markErrors);
			}
		} else if (strcmp(commandName, "save") == 0) {/* **** Save command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					1) == -1) {
			} else {
				saveToFile(*PPGame, firstArg, *gameMode);
			}
		} else if (strcmp(commandName, "hint") == 0) {/* **** Hint command **** */
			int intX, intY;
			if (checkMode((gameMode), 2, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					2) == -1) {
			} else if (checkRange(PPGame, &intX, &intY, firstArg, secondArg)
					== -1) {
			} else if (isErroneous((*PPGame))) {
				printError(ErroneousBoard);
			} else {
				hint(PPGame, intY, intX);
			}
		} else if (strcmp(commandName, "guess_hint") == 0) {/* **** Guess hint command **** */
			int intX, intY,checkResult=2;
			if (checkMode((gameMode), 2, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					2) == -1) {
			} else if (checkRange(PPGame, &intX, &intY, firstArg, secondArg)
					== -1) {
			} else if (isErroneous((*PPGame))) {
				printError(ErroneousBoard);
			} else if ((*PPGame)->fixedBoard[intY * (*PPGame)->N + intX] == 1) {
				printError(CellIsFixed);
			} else if ((*PPGame)->gameBoard[intY * (*PPGame)->N + intX] != 0) {
				printError(CellIsNotEmpty);
			} else {
				if ((checkResult=solve((*PPGame), NULL, 0, 0, intY, intX, 0)) ==0) {
					printError(UnsolvableBoard);
				}
				if(checkResult==-1){
					printError(guessHintFailed);
				}
			}
		} else if (strcmp(commandName, "num_solutions") == 0) {/* **** Num solutions command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else if (isErroneous((*PPGame))) {
				printError(ErroneousBoard);
			} else {
				int solNum;
				if(isComplete(*PPGame)&&correctSolution(*PPGame)){
					solNum=1;
				}
				else{
				solNum = numOfSolutions((*PPGame));
				}
				printf("The number of solutions for the current board is %d\n",
						solNum);
			}
		} else if (strcmp(commandName, "autofill") == 0) {/* **** Autofill command **** */
			if (checkMode(gameMode, 2, -1) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else if (isErroneous((*PPGame))) {
				printError(ErroneousBoard);
			} else {
				autofill(PPGame, gameMode, markErrors);
			}
		} else if (strcmp(commandName, "reset") == 0) {/* **** Reset command **** */
			if (checkMode((gameMode), 1, 2) == -1) {
			} else if (checkParameters(firstArg, secondArg, thirdArg, fourthArg,
					0) == -1) {
			} else {
				reset(PPGame, gameMode, markErrors);
			}
		} else if (strcmp(commandName, "exit") == 0) {/* **** Exit command **** */
			if (checkParameters(firstArg, secondArg, thirdArg, fourthArg, 0)
					== -1) {
			} else {
				printf("Exiting...\n");
				freeGame(*PPGame);
				exit(1);
			}
		} else {/*command was't matched*/
			printError(InvalidCommand);
		}
	} else {
		printError(InvalidCommand);
	}
}
