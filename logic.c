#include "logic.h"
#include <stdio.h>
#include <stdlib.h>

/* define a new pos structure to hold integer values. */
typedef struct intpos intpos;
struct intpos {
  int r, c;
};

/* make a new game with standard outlay. */
game *new_game(unsigned int nrows, unsigned int ncols, enum type type) {
  if (nrows <= 2 || ncols <= 2) {
    fprintf(stderr, "Insufficient rows or cols\n");
    exit(1);
  }
  game *res = (game *)malloc(sizeof(game));
  res->next = BLACK_NEXT;
  board *b = board_new(nrows, ncols, type);
  pos p1 = make_pos(nrows / 2 - 1, ncols / 2 - 1);
  pos p2 = make_pos(nrows / 2, ncols / 2 - 1);
  pos p3 = make_pos(nrows / 2 - 1, ncols / 2);
  pos p4 = make_pos(nrows / 2, ncols / 2);
  board_set(b, p1, WHITE);
  board_set(b, p2, BLACK);
  board_set(b, p3, BLACK);
  board_set(b, p4, WHITE);
  res->b = b;
  return res;
}

/* free the game and the board in it. */
void game_free(game *g) {
  board_free(g->b);
  free(g);
}

/* change a pos to intpos */
intpos pos_transition(pos p) {
  intpos res;
  res.r = p.r;
  res.c = p.c;
  return res;
};

/* given row and col number, make a new intpos. */
intpos make_intpos(int r, int c) {
  intpos res;
  res.r = r;
  res.c = c;
  return res;
}

/* return true is the intpos is valid. */
int valid_pos(game *g, intpos p) {
  if (p.c < g->b->ncols && p.r < g->b->nrows && p.c >= 0 && p.r >= 0)
    return 1;
  else
    return 0;
}

/* change BLACK_NEXT to WHITE_NEXT, and WHITE_NEXT to BLACK_NEXT. */
turn turn_change(turn t) {
  switch (t) {
  case BLACK_NEXT:
    return WHITE_NEXT;
  case WHITE_NEXT:
    return BLACK_NEXT;
  default:
    fprintf(stderr, "wrong turn value");
    exit(1);
  }
}

/* change BLACK to WHITE and WHITE to BLACK. */
square change(square s) {
  if (s == BLACK)
    return WHITE;
  if (s == WHITE)
    return BLACK;
}

/* change BLACK_NEXT to BLACK and WHITE_NEXT to WHITE. */
square turn_to_square(turn t) {
  switch (t) {
  case BLACK_NEXT:
    return BLACK;
  case WHITE_NEXT:
    return WHITE;
  default:
    fprintf(stderr, "wrong turn name");
    exit(1);
  }
}

/* return the poslist need to be flipped given by a direction
 * drow and dcol denote the change of row and col number in that directon. */
poslist *flips_side(game *g, pos p, int drow, int dcol) {
  poslist *list = NULL;
  intpos curr = make_intpos(p.r + drow, p.c + dcol);
  if (valid_pos(g, curr)) {
    while (valid_pos(g, curr)) {
      if (board_get(g->b, make_pos(curr.r, curr.c)) ==
          change(turn_to_square(g->next))) {
        list = pos_cons(make_pos(curr.r, curr.c), list);
        curr.r = curr.r + drow;
        curr.c = curr.c + dcol;
      } else
        break;
    }
    if (valid_pos(g, curr) && (board_get(g->b, make_pos(curr.r, curr.c)) ==
                               turn_to_square(g->next))) {
      return list;
    } else
      return NULL;
  } else
    return NULL;
}

/* return the poslist need to be flipped. */
poslist *flips(game *g, pos p) {
  poslist *l1 = pos_append(
      flips_side(g, p, 0, 1),
      pos_append(flips_side(g, p, 0, -1),
                 pos_append(flips_side(g, p, 1, 0), flips_side(g, p, -1, 0))));
  poslist *l2 = pos_append(
      flips_side(g, p, 1, 1),
      pos_append(flips_side(g, p, -1, -1),
                 pos_append(flips_side(g, p, 1, -1), flips_side(g, p, -1, 1))));
  return pos_append(l1, l2);
}

/* return true if the given pos is a legal move for the current player. */
int outflanks(game *g, pos p) {
  if (board_get(g->b, p) != EMPTY) {
    return 0;
  }
  if (valid_pos(g, pos_transition(p)) && flips(g, p)) {
    return 1;
  } else
    return 0;
}

/* if a pos is legal move, flip the board and modify the game data. */
void apply_move(game *g, pos p) {
  if (!outflanks(g, p)) {
    fprintf(stderr, "Illegal Move\n");
    return;
  }
  poslist *list = flips(g, p);
  poslist *curr = list;
  board_set(g->b, p, turn_to_square(g->next));
  for (curr; curr != NULL; curr = curr->next) {
    board_set(g->b, curr->p, turn_to_square(g->next));
  }
  g->next = turn_change(g->next);
}

/* return true if the current player does not have legal moves */
int skip(game *g) {
  for (unsigned i = 0; i < g->b->nrows; i++) {
    for (unsigned j = 0; j < g->b->ncols; j++) {
      if (outflanks(g, make_pos(i, j))) {
        return 0;
      }
    }
  }
  return 1;
}

/* return true if the game is over. */
int game_over(game *g) {
  if (!skip(g)) {
    return 0;
  }
  g->next = turn_change(g->next);
  return skip(g);
}

/* return the game outcome if the game is over. */
outcome game_outcome(game *g) {
  if (game_over(g)) {
    int countb = 0, countw = 0;
    for (unsigned i = 0; i < g->b->nrows; i++) {
      for (unsigned j = 0; j < g->b->ncols; j++) {
        if (board_get(g->b, make_pos(i, j)) == BLACK) {
          countb++;
        }
        if (board_get(g->b, make_pos(i, j)) == WHITE) {
          countw++;
        }
      }
    }
    if (countb > countw)
      return BLACK_WIN;
    if (countb < countw)
      return WHITE_WIN;
    if (countb == countw)
      return DRAW;
  }
}
