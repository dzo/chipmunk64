

extern void init_X_screen(void);
extern void recolor_log_cursors(int color, int force);
extern void choose_log_cursor(int curs);

struct ext_proc {
  char *name;
  Void (*proc)();
};

