/*ParserAux module - valid checks are performed for commends parameters received from the user in the parser model.
 */

#ifndef PARSERAUX_H_
#define PARSERAUX_H_

int isInteger(char * x, int isIndex);
int inRange(int x, int y, int z);
float getThreshold(char* str);
void errorPrinter(Game *  game);
int checkParameters(char* firstArg, char* secondArg, char* thirdArg,
		char* fourthArg, int amount);
int checkMode(int* gameMode, int legal1, int legal2);
int checkRange(Game ** PPGame, int* intX, int* intY, char* firstArg,
		char* secondArg);
int checkForGenerate(Game** PPGame,int* intX,int* intY, char* firstArg,char* secondArg,int* emptyCells);

#endif /* PARSERAUX_H_ */
