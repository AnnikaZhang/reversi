struct pos {
    unsigned int r, c;
};

typedef struct pos pos;

typedef struct poslist poslist;

struct poslist {
    pos p;
    poslist *next;
};

/* make a pos given row and col number (return a pos, not pointer). */
pos make_pos(unsigned int r, unsigned int c);

/* cons a poslist. */
poslist *pos_cons(pos p, poslist *ps);

/* free the pointers in a poslist. */
void poslist_free(poslist *ps);

/* return a poslist combing the given two polists. */
poslist *pos_append(poslist *l1, poslist *l2);

/* helper: return the length of a poslist. */
unsigned int poslist_len(poslist *ps);

/* print a poslist. */
void poslist_show(poslist *ps);
