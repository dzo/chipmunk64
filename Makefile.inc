CHIPMUNKCC = clang -Os
CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0 -Wno-deprecated-non-prototype
#CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0
CHIPMUNKCC = emcc --bind -sUSE_SDL=2  -sUSE_SDL_GFX=2 -sASYNCIFY -s ASYNCIFY_STACK_SIZE=512000
