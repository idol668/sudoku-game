/*ErrorHandler module -  designed to notify the user of errors detected during the program run,
 as well as to inform the cause of the errors .*/

#include <stdio.h>
#include <stdlib.h>
#include "MainAux.h"
#include "ErrorHandler.h"

/* This function notifies the user which error occurred during the program, using enum "Error" and switch statement.*/
void printError(Error err) {

	switch (err) {
	case CellIsFixed: {
		printf("[Error]: cell is fixed\n");
		break;
	}
	case IlegalPath: {
		printf("[Error]: illegal path\n");
		break;
	}
	case notAvailable: {
		printf("[Error]: the command isn't available in this mode\n");
		break;
	}
	case CellIsNotEmpty: {
		printf("[Error]: the cell already contains a value\n");
		break;
	}
	case WMarkErrors: {
		printf("[Error]: the first argument should be 0 or 1\n");
		break;
	}
	case FirstNotInRange: {
		printf("[Error]: first argument should be an Integer\n");
		break;
	}
	case SecondNotInRange: {
		printf("[Error]: second argument should be an Integer\n");
		break;
	}
	case ThirdNotInRange: {
		printf("[Error]: third argument should be an Integer\n");
		break;
	}
	case limitedRange: {
		printf(
				"[Error]: The multiplication of row and column sizes should be less than 99\n");
		break;
	}
	case InvalidCommand: {
		printf("[Error]: invalid command\n");
		break;
	}
	case FewParams: {
		printf("[Error]: not enough parameters were entered\n");
		break;
	}
	case ManyParams: {
		printf("[Error]: too many parameters were entered\n");
		break;
	}
	case FileOpenFailure: {
		printf("[Error]: File open failed\n");
		break;
	}
	case ManyChar: {
		printf("[Error]: too many char in file\n");
		break;
	}
	case RedoUnavailable: {
		printf("[Error]: Redo command is not possible\n");
		break;
	}
	case UndoUnavailable: {
		printf("[Error]: Undo command is not possible\n");
		break;
	}
	case ErroneousBoard: {
		printf("[Error]: Board is erroneous\n");
		break;
	}
	case UnsolvableBoard: {
		printf("[Error]: board is unsolvable\n");
		break;
	}
	case InputTooLong: {
		printf("[Error]: too many characters where entered in a single line\n");
		break;
	}
	case GuessHintFailed: {
		printf("[Error]: chosen cell already contains value\n");
		break;
	}
	case GuessFailed: {
		printf("[Error]: Could not guess hint\n");
		break;
	}
	case MemoryFailed: {
		printf("[Error]:allocating memory failed\n");
		break;
	}
	case GenerationFailed: {
		printf("[Error]: generate failed\n");
		break;
	}
	case loadFailded: {
		printf("[Error]: load file failed \n");
		break;
	}
	case FileSaveFailure: {
		printf("[Error]: File save failed\n");
		break;
	}
	case FileCloseFailure: {
		printf("[Error]: File close failed\n");
		break;
	}
	case NoParams: {
		printf("[Error]: This command does not require parameters \n");
		break;
	}
	case FileNotInRange: {
		printf("[Error]: value from text file is in the wrong range\n");
		break;
	}
	case EmptyFix: {
		printf("[Error]: empty cell can not be fixed\n");
		break;
	}
	case ErroneousFix: {
		printf("[Error]: erroneous fixed cells in file\n");
		break;
	}
	case firstfloat: {
		printf(
				"[Error]: first arguments should be a floating point number between 0 and 1\n");
		break;
	}
	case positiveValue: {
		printf("[Error]: The input should be positive\n");
		break;
	}
	case validateFailed: {
		printf("[Error]: validate failed\n");
		break;
	}
	case guessFailed: {
		printf("[Error]: guess failed\n");
		break;
	}
	case hintFailed: {
		printf("[Error]: hint failed\n");
		break;
	}
	case guessHintFailed: {
		printf("[Error]: guess_hint failed\n");
		break;
	}
	case saveFailed: {
		printf("[Error]: validation of board failed\n");
		break;
	}
	}
}
/*This function notifies the user about the error's cause details of the previous error occurred */

void printDetails(detail err) {
	switch (err) {
	case (needoneParameter): {
		printf("This function requires one parameter \n");
		break;
	}
	case (needtwoParameter): {
		printf("This function requires two parameters \n");
		break;
	}
	case (needthreeParameter): {
		printf("This function requires three parameters \n");
		break;
	}
	case BlockSize: {
		printf(" Block sizes should be integers");
		break;
	}
	case Invalidchar: {
		printf(" File must contain only integers");
		break;
	}
	case Solve: {
		printf("The command available only in Solve\n");
		break;
	}
	case EditSolve: {
		printf("The command available in Edit and Solve\n");
		break;
	}
	case Edit: {
		printf("The command available only in Edit\n");
		break;
	}
	case exiting: {
		printf("Exiting...\n");
		break;
	}
	case InvalidFile: {
		printf("Wrong format of text\n");
		break;
	}

	}
}

