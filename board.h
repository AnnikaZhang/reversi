#include "pos.h"

enum square { EMPTY, BLACK, WHITE };

typedef enum square square;

union board_rep {
    enum square **cells;
    unsigned int *bits;
};

typedef union board_rep board_rep;

enum type { CELLS, BITS };

struct board {
    unsigned int nrows, ncols;
    enum type type;
    board_rep u;
};

typedef struct board board;

/* make a new board given dimensions and representation type. */
board *board_new(unsigned int nrows, unsigned int ncols, enum type type);

/* free the pointers in board. */
void board_free(board *b);

/* visualize the square information in a board. */
void board_show(board *b);

/* given a board and a pos, return the square information. */
square board_get(board *b, pos p);

/* change the pos in a board to a given square. */
void board_set(board *b, pos p, square s);
