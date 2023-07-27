#ifndef MARKRELEASE_H
#define MARKRELEASE_H

#ifdef MARKRELEASE_G
# define vextern
#else
# define vextern extern
#endif

struct record {
  struct record *prev;
} *mark_first_mallocced_info;

extern void mark(struct record **);
extern void release(struct record **);
extern char *fakemalloc(long);

#define malloc(x) fakemalloc(x)

#undef vextern

#endif
