#include "SPBufferset.h"
#include "Game.h"
#include "MainAux.h"
#include "FileManage.h"
#include "Parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "ExhaustiveBacktracker.h"
#include "Execution.h"
/*initiate mark errors and game mode variables for the game, as well as a pointer to the game to be use through the game.
 * prints and initial message and calling userCommand for continuous command read and execute from the user. */
int main() {
	int markErrors = 1;
	int *ptrMarkErrors = &markErrors;
	int gameMode = 0;
	int *ptrGameMode = &gameMode;
	Game * game = NULL;
	SP_BUFF_SET();
	initialMessage();
	while (1) {
		userCommand(&game, ptrGameMode, ptrMarkErrors);
	}
}

