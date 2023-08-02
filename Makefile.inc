ifdef WIN32
CHIPMUNKCC = clang -O3
CHIPMUNKCC = x86_64-w64-mingw32-gcc  -Os -L../../SDL2-2.28.1/x86_64-w64-mingw32/lib -fomit-frame-pointer -DLACK_GETPWNAM -D__GCC__ -I../../SDL2-2.28.1/x86_64-w64-mingw32/include/
CHIPMUNKFLAGS =  -DOS2
AR=llvm-ar
RANLIB=llvm-ranlib
LIBX11 =  -lSDL2 -lopengl32
endif
ifdef WASM
CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0 -Wno-deprecated-non-prototype -O3 
#-O1 -g
CHIPMUNKCC = emcc  -sUSE_SDL=2  -sUSE_SDL_GFX=2
AR=emar
RANLIB=emranlib
LIBX11 =  -lSDL2 -lSDL2_gfx -lGL
# --bind -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200
endif
ifdef LINUX
CHIPMUNKCC = clang -O3
#CHIPMUNKCC = x86_64-w64-mingw32-gcc  -Os -L../../SDL2-2.28.1/x86_64-w64-mingw32/lib -fomit-frame-pointer -DF_OK=0 -DLACK_GETPWNAM -D__GCC__ -I../../SDL2-2.28.1/x86_64-w64-mingw32/include/
CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0
LIBX11 =  -lSDL2 -lGL
AR=llvm-ar
RANLIB=llvm-ranlib
endif