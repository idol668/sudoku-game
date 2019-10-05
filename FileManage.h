/*File handling module - responsible for dealing with saving and loading files
 * - checking the board valid for saving in edit mode
 * - checking the file contain legal values before loading
 */
#ifndef FILEMANAGE_H_
#define FILEMANAGE_H_
#include "Game.h"

int saveToFile(Game* game, char* filePath,int gameMode);
int loadGame(Game ** PPGame, char* filePath, int newGameMode);

#endif /* FILEMANAGE_H_ */
