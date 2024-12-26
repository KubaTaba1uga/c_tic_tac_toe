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
 *    PUBLIC API
 ******************************************************************************/
struct GameOps {
  int (*start)(void);
  void (*quit)(void);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameOps *get_game_ops(void);

#endif // GAME_H
