
#include <stdio.h>
#include <ctype.h>
#include <p2c/p2c.h>

typedef union swap2 {
        short ival;
        unsigned char c[2];
} swap2;

typedef union swap4 {
        long ival;
        unsigned char c[4];
} swap4;

extern short shortsw(short sh);
extern short getshortsw(char * c);

extern long getintsw(char * c);
extern long intsw(long ii);
extern short reverse(short s);











