#include "pos.h"
#include <stdio.h>
#include <stdlib.h>

/* make a pos given row and col number (return a pos, not pointer). */
pos make_pos(unsigned int r, unsigned int c) {
  pos res;
  res.c = c;
  res.r = r;
  return res;
}

/* cons a poslist. */
poslist *pos_cons(pos p, poslist *ps) {
  poslist *res = (poslist *)malloc(sizeof(poslist));
  res->p = p;
  res->next = ps;
  return res;
}

/* return a poslist combing the given two polists. */
poslist *pos_append(poslist *l1, poslist *l2) {
  if (!l1)
    return l2;
  else
    return pos_cons(l1->p, pos_append(l1->next, l2));
}

/* free the pointers in a poslist. */
void poslist_free(poslist *ps) {
  if (ps != NULL) {
    poslist_free(ps->next);
    free(ps);
  }
}

unsigned int poslist_len(poslist *ps) {
  poslist *cur = ps;
  unsigned res = 0;
  while (cur) {
    res++;
    cur = cur->next;
  }
  return res;
}

/* print a poslist. */
void poslist_show(poslist *ps) {
  while (ps) {
    printf("(%d,%d) ", ps->p.r, ps->p.c);
    ps = ps->next;
  }
}
