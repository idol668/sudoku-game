/* Parser module - designed for parsing user command and react accordingly, check validity of command
 * and send to execution if it valid for game status,and parameters received using he Execution module when needed.
 */

#ifndef PARSER_H_
#define PARSER_H_

void userCommand(Game ** PPGame, int * gameMode, int * markErrors);

#endif /* PARSER_H_ */
