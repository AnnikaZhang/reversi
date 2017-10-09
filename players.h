#ifndef __PLAYERS_H
#define __PLAYERS_H

#include "logic.h"

struct edge_corner_weight {
  unsigned int edge_weight;
  unsigned int corner_weight;
};

union heuristic_config {
  unsigned int edge_weight;
  struct edge_corner_weight edge_corner_weight;
};

typedef union heuristic_config heuristic_config;

struct minimax_config {
  int (*heuristic)(game *, heuristic_config);
  heuristic_config hc;
  unsigned int ply;
};

typedef struct minimax_config minimax_config;

union strategy_config {
  minimax_config minimax_config;
};

typedef union strategy_config strategy_config;

game *cpy_game(game *);

pos first_move(game *g, strategy_config unused);

pos human(game *g, strategy_config unused);

pos immediate_tactics(game *g, strategy_config unused);

int did_i_win(game *g, heuristic_config unused);

int piece_counting(game *g, heuristic_config unused);

int prefer_edges(game *g, heuristic_config hc);

int prefer_edges_and_corners(game *g, heuristic_config hc);

pos minimax(game *g, strategy_config sc);

#endif
