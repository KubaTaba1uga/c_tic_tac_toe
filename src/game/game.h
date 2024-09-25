#ifndef GAME_H
#define GAME_H
/*******************************************************************************
 * @file game.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
enum Users {
  UserNone = 0,
  User1,
  User2,
  UserMax,
};

struct GameOps {};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameOps game_ops;

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData *init_game_reg_p;

#endif // GAME_H
