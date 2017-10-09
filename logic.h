#include "board.h"

enum turn { BLACK_NEXT, WHITE_NEXT };

typedef enum turn turn;

enum outcome { BLACK_WIN, WHITE_WIN, DRAW };

typedef enum outcome outcome;

struct game {
    board *b;
    turn next;
};

typedef struct game game;

/* make a new game with standard outlay. */
game *new_game(unsigned int nrows, unsigned int ncols, enum type type);

/* return true if the given pos is a legal move for the current player. */
int outflanks(game *g, pos p);

/* return the poslist need to be flipped. */
poslist *flips(game *g, pos p);

/* if a pos is legal move, flip the board and modify the game data. */
void apply_move(game *g, pos p);

/* return true if the game is over. */
int game_over(game *g);

/* return the game outcome if the game is over. */
outcome game_outcome(game *g);

/* free the game and the board in it. */
void game_free(game *g);

/* return ture if the current player has no legal move. */
int skip(game *g);
