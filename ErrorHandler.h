/*ErrorHandler module -  designed to notify the user of errors detected during the program run,
  as well as to inform the cause of the errors  using "error" and "detail" enums.*/
#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

/* enum error - used for classify errors  */
typedef enum _error {

	CellIsFixed,
	IlegalPath,
	notAvailable,
	CellIsNotEmpty,
	WMarkErrors,
	FirstNotInRange,
	SecondNotInRange,
	ThirdNotInRange,
	limitedRange,
	InvalidCommand,
	FewParams,
	ManyParams,
    FileOpenFailure,
	loadFailded,
	ManyChar,
	RedoUnavailable,
	UndoUnavailable,
	ErroneousBoard,
	UnsolvableBoard,
	InputTooLong,
	GuessHintFailed,
	GuessFailed,
	MemoryFailed,
	GenerationFailed,
	FileSaveFailure,
	FileCloseFailure,
	NoParams,
	FileNotInRange,
	EmptyFix,
	ErroneousFix,
	firstfloat,
	positiveValue,
	validateFailed,
	guessFailed,
	hintFailed,
	guessHintFailed,
	saveFailed
} Error;

/*enum detail - used to inform the user the cause of an error  */
typedef enum _details{
needoneParameter,
needtwoParameter,
needthreeParameter,
BlockSize,
Invalidchar,
InvalidFile,
Solve,
EditSolve,
Edit,
exiting
}detail;
void printError(Error err);
void printDetails(detail err);
#endif /* ERRORHANDLER_H_ */
