#include "board.h"
#include <stdio.h>
#include <stdlib.h>

/* make a new board given dimensions and representation type. */
board *board_new(unsigned int nrows, unsigned int ncols, enum type type) {
    board *res = (board *)malloc(sizeof(board));
    res->nrows = nrows;
    res->ncols = ncols;
    res->type = type;
    switch (type) {
    case CELLS:
        res->u.cells = (square **)malloc(nrows * sizeof(square *));
            for (unsigned i = 0; i < nrows; i++) {
                res->u.cells[i] = (square *)malloc(ncols * sizeof(square));
                for (unsigned j = 0; j < ncols; j++) {
                    res->u.cells[i][j] = EMPTY;
                }
            }
        return res;
    case BITS:
        res->u.bits = (unsigned *)malloc(((nrows * ncols) / 16 + 1) * 4);
        int len = (nrows * ncols) / 16 + 1;
        for (unsigned k = 0; k < len; k++) {
            res->u.bits[k] = 0;
        }
        return res;
    default:
        fprintf(stderr, "wrong type");
        exit(1);
    }
}

/* free the pointers in board. */
void board_free(board *b) {
    switch (b->type) {
    case CELLS:
        for (unsigned i = 0; i < b->nrows; i++) {
            free(b->u.cells[i]);
        }
        free(b->u.cells);
        break;
    case BITS:
        free(b->u.bits);
        break;
    }
    free(b);
}

/* given a board and a pos, return the square information. */
square board_get(board *b, pos p) {
    if (b->type == CELLS) {
        return b->u.cells[p.r][p.c];
    }
    if (b->type == BITS) {
        int n = (p.r * b->ncols + p.c) / 16;
        int m = (p.r * b->ncols + p.c) % 16;
        switch ((b->u.bits[n] >> (m * 2)) & 3) {
        case 0:
            return EMPTY;
        case 1:
            return BLACK;
        case 2:
            return WHITE;
        default:
            fprintf(stderr, "wrong storage");
            exit(1);
        }
    }
}

/* helper, print the square information. */
void print_square(square n) {
    switch (n) {
    case EMPTY:
        printf(" ");
        break;
    case BLACK:
        printf("x");
        break;
    case WHITE:
        printf("o");
        break;
    default:
        fprintf(stderr, "wrong tag");
        exit(1);
    }
}

/* print the index, from 1-9, A-Z to a-z. */
void print_index(unsigned i) {
    if (i < 10) {
        printf("%d", i);
    } else if (i < 36) {
        printf("%c", i - 10 + 65);
    } else if (i < 62) {
        printf("%c", i - 36 + 97);
    } else {
        printf("?");
    }
}

/* visualize the square information in a board. */
void board_show(board *b) {
    for (unsigned i = 0; i <= b->nrows; i++) {
        for (unsigned j = 0; j <= b->ncols; j++) {
            if (i == 0) {
                if (j != 0) {
                    print_index(j - 1);
                } else {
                printf(" ");
            }
            } else {
            if (j == 0) {
                print_index(i - 1);
            } else {
                print_square(board_get(b, make_pos(i - 1, j - 1)));
            }
         }
     }
      printf("\n");
    }
}

/* change the pos in a board to a given square. */
void board_set(board *b, pos p, square s) {
    if (b->type == CELLS) {
        b->u.cells[p.r][p.c] = s;
    } else {
        int n = (p.r * b->ncols + p.c) / 16;
        int m = (p.r * b->ncols + p.c) % 16;
        if (board_get(b, p) != EMPTY) {
            b->u.bits[n] -= (((b->u.bits[n] >> (m * 2)) & 3) << (m * 2));
        }
        b->u.bits[n] = b->u.bits[n] | s << (m * 2);
    }
}
