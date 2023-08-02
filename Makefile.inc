ifndef WASM
CHIPMUNKCC = clang -O3
CHIPMUNKCC = x86_64-w64-mingw32-gcc  -Os -fomit-frame-pointer -DF_OK=0 -DLACK_GETPWNAM -D__GCC__ -I../../SDL2-2.28.1/x86_64-w64-mingw32/include/
#CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0
AR=llvm-ar
RANLIB=llvm-ranlib
else
CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0 -Wno-deprecated-non-prototype -O3
#-O1 -g
CHIPMUNKCC = emcc  -sUSE_SDL=2  -sUSE_SDL_GFX=2
AR=emar
RANLIB=emranlib
# --bind -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200
endif
