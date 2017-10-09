#include "players.h"
#include <stdio.h>
#include <stdlib.h>

/* return the first available move.*/
pos first_move(game *g, strategy_config unused) {
  for (unsigned int i = 0; i < g->b->nrows; i++) {
    for (unsigned int j = 0; j < g->b->ncols; j++) {
      if (outflanks(g, make_pos(i, j))) {
        return make_pos(i, j);
        break;
      }
    }
  }
}

/* this function reverts characters into designated integer index. */
int convert_index(char c) {
  if (c >= 48 && c <= 57)
    return c - 48;
  if (c >= 65 && c <= 90)
    return c - 55;
  if (c >= 97 && c <= 122)
    return c - 61;
  else
    return -1;
}

/* this function reads move from input. */
pos human(game *g, strategy_config unused) {
  char c1, c2;
  while (1) {
    switch (g->next) {
    case BLACK_NEXT:
      printf("BLACK: \n");
      break;
    case WHITE_NEXT:
      printf("WHITE: \n");
      break;
    }
    scanf(" %c%c", &c1, &c2);
    if (convert_index(c1) >= 0 && convert_index(c1) < g->b->nrows &&
        convert_index(c2) >= 0 && convert_index(c2) < g->b->ncols) {
      pos res = make_pos(convert_index(c1), convert_index(c2));
      if (outflanks(g, res))
        return res;
    }
  }
}

/* occupy corners and edges if available, else
choose the position with max number of flips. */
/* in last minute I realized I didn't prioritize max flip number of the moves on
 * corners. But sadly I don't have enough time to change it.... sad face*/
pos immediate_tactics(game *g, strategy_config unused) {
  if (!game_over(g)) {
    if (outflanks(g, make_pos(0, 0)))
      return make_pos(0, 0);
    if (outflanks(g, make_pos(0, g->b->ncols - 1)))
      return make_pos(0, g->b->ncols - 1);
    if (outflanks(g, make_pos(g->b->nrows - 1, 0)))
      return make_pos(g->b->nrows - 1, 0);
    if (outflanks(g, make_pos(g->b->nrows - 1, g->b->ncols - 1)))
      return make_pos(g->b->nrows - 1, g->b->ncols - 1);
    for (unsigned i = 0; i < g->b->ncols - 1; i++) {
      if (outflanks(g, make_pos(0, i)))
        return make_pos(0, i);
      if (outflanks(g, make_pos(g->b->nrows - 1, i)))
        return make_pos(g->b->nrows - 1, i);
    }
    for (unsigned j = 0; j < g->b->nrows - 1; j++) {
      if (outflanks(g, make_pos(j, 0)))
        return make_pos(j, 0);
      if (outflanks(g, make_pos(j, g->b->ncols - 1)))
        return make_pos(j, g->b->ncols - 1);
    }
    pos res = make_pos(0, 0);
    int max = 0;
    for (unsigned i = 1; i < g->b->nrows - 1; i++) {
      for (unsigned j = 1; j < g->b->ncols - 1; j++) {
        if (outflanks(g, make_pos(i, j)) &&
            poslist_len(flips(g, make_pos(i, j))) > max) {
          max = poslist_len(flips(g, make_pos(i, j)));
          res.r = i;
          res.c = j;
        }
      }
    }
    return res;
  }
}

/* a silly way to evaluate a board: win=1, lose=-1, and else=0. */
int did_i_win(game *g, heuristic_config unused) {
  if (game_over(g)) {
    switch (game_outcome(g)) {
    case BLACK_WIN:
      return 1;
    case WHITE_WIN:
      return -1;
    case DRAW:
      return 0;
    }
  }
  return 0;
}

/* count the number of black pieces minus the number of white pieces. */
int piece_counting(game *g, heuristic_config unused) {
  int res = 0;
  for (unsigned i = 0; i < g->b->nrows; i++) {
    for (unsigned j = 0; j < g->b->ncols; j++) {
      switch (board_get(g->b, make_pos(i, j))) {
      case BLACK:
        res++;
        break;
      case WHITE:
        res--;
        break;
      case EMPTY:
        break;
      }
    }
  }
  return res;
}

/* given the edge weight, sums up the interior numbers and weighted edge
 * numbers*/
int prefer_edges(game *g, heuristic_config hc) {
  int res = 0;
  for (unsigned int i = 0; i < g->b->nrows; i++) {
    for (unsigned int j = 0; j < g->b->ncols; j++) {
      if (i == 0 || j == 0 || i == g->b->nrows - 1 || j == g->b->ncols - 1) {
        switch (board_get(g->b, make_pos(i, j))) {
        case BLACK:
          res += hc.edge_weight;
          break;
        case WHITE:
          res -= hc.edge_weight;
          break;
        case EMPTY:
          break;
        }
      } else {
        switch (board_get(g->b, make_pos(i, j))) {
        case BLACK:
          res += 1;
          break;
        case WHITE:
          res -= 1;
          break;
        case EMPTY:
          break;
        }
      }
    }
  }
  return res;
}

/* sums of the interior pieces and weighted edge and corner numbers. */
int prefer_edges_and_corners(game *g, heuristic_config hc) {
  int res = 0;
  for (unsigned int i = 0; i < g->b->nrows; i++) {
    for (unsigned int j = 0; j < g->b->ncols; j++) {
      if ((i == 0 && j == 0) || (i == 0 && j == g->b->ncols - 1) ||
          (i == g->b->nrows - 1 && j == 0) ||
          (i == g->b->nrows - 1 && j == g->b->ncols - 1)) {
        switch (board_get(g->b, make_pos(i, j))) {
        case BLACK:
          res += hc.edge_corner_weight.corner_weight;
          break;
        case WHITE:
          res -= hc.edge_corner_weight.corner_weight;
          break;
        case EMPTY:
          break;
        }
      } else if (i == 0 || i == g->b->nrows - 1 || j == 0 ||
                 j == g->b->ncols - 1) {
        switch (board_get(g->b, make_pos(i, j))) {
        case BLACK:
          res += hc.edge_corner_weight.edge_weight;
          break;
        case WHITE:
          res -= hc.edge_corner_weight.edge_weight;
          break;
        case EMPTY:
          break;
        }
      } else {
        switch (board_get(g->b, make_pos(i, j))) {
        case BLACK:
          res += 1;
          break;
        case WHITE:
          res -= 1;
          break;
        case EMPTY:
          break;
        }
      }
    }
  }
  return res;
}

/* helper: copy the game. */
game *cpy_game(game *g) {
  game *res = new_game(g->b->nrows, g->b->ncols, g->b->type);
  for (unsigned i = 0; i < g->b->nrows; i++) {
    for (unsigned j = 0; j < g->b->ncols; j++) {
      board_set(res->b, make_pos(i, j), board_get(g->b, make_pos(i, j)));
    }
  }
  res->next = g->next;
  return res;
}

/*helper: recursively calls for minimax_eval. */
int minimax_recursive(game *g, minimax_config mc, unsigned int ply) {
  if (ply == 0) {
    return mc.heuristic(g, mc.hc);
  }
  if (skip(g)) {
    game *g2 = cpy_game(g);
    switch (g->next) {
    case BLACK_NEXT:
      g2->next = WHITE_NEXT;
      return minimax_recursive(g2, mc, ply - 1);
    case WHITE_NEXT:
      g2->next = BLACK_NEXT;
      return minimax_recursive(g2, mc, ply - 1);
    }
  }
  int res;
  switch (g->next) {
  case BLACK_NEXT:
    res = -10000;
    break;
  case WHITE_NEXT:
    res = 10000;
    break;
  }
  for (unsigned i = 0; i < g->b->nrows; i++) {
    for (unsigned j = 0; j < g->b->ncols; j++) {
      if (outflanks(g, make_pos(i, j))) {
        game *g2 = cpy_game(g);
        apply_move(g2, make_pos(i, j));
        if (game_over(g2)) {
          return mc.heuristic(g2, mc.hc);
        }
        switch (g->next) {
        case BLACK_NEXT:
          if (minimax_recursive(g2, mc, ply - 1) > res)
            res = minimax_recursive(g2, mc, ply - 1);
          break;
        case WHITE_NEXT:
          if (minimax_recursive(g2, mc, ply - 1) < res)
            res = minimax_recursive(g2, mc, ply - 1);
          break;
        }
        game_free(g2);
      }
    }
  }
  return res;
}

/* given a board, return the minimax evaluation. */
int minimax_eval(game *g, minimax_config mc) {
  return minimax_recursive(g, mc, mc.ply - 1);
}

/* scan over the board and return the min or max choice. */
pos minimax(game *g, strategy_config sc) {
  if (sc.minimax_config.ply == 0) {
    fprintf(stderr, "ply can't be 0\n");
    exit(1);
  }
  if (game_over(g)) {
    fprintf(stderr, "Game is already over.\n");
    exit(1);
  }
  unsigned int r;
  unsigned int c;
  int candidate;
  switch (g->next) {
  case BLACK_NEXT:
    candidate = -10000;
    break;
  case WHITE_NEXT:
    candidate = 10000;
    break;
  }
  for (int i = 0; i < g->b->nrows; i++) {
    for (int j = 0; j < g->b->ncols; j++) {
      if (outflanks(g, make_pos(i, j))) {
        game *g2 = cpy_game(g);
        apply_move(g2, make_pos(i, j));
        switch (g->next) {
        case BLACK_NEXT:
          if (minimax_eval(g2, sc.minimax_config) > candidate) {
            candidate = minimax_eval(g2, sc.minimax_config);
            r = i;
            c = j;
          }
          break;
        case WHITE_NEXT:
          if (minimax_eval(g2, sc.minimax_config) < candidate) {
            candidate = minimax_eval(g2, sc.minimax_config);
            r = i;
            c = j;
          }
          break;
        }
        game_free(g2);
      }
    }
  }
  return make_pos(r, c);
}
