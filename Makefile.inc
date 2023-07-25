CHIPMUNKCC = clang -Os
CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0
CHIPMUNKCC = emcc --bind -sUSE_SDL=2  -s USE_SDL_GFX=2
