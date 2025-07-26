ifdef WIN32
CHIPMUNKCC = clang -O3
CHIPMUNKCC = x86_64-w64-mingw32-gcc -Os -L../../SDL2-2.28.1/x86_64-w64-mingw32/lib -fomit-frame-pointer -DLACK_GETPWNAM -D__GCC__ -I../../SDL2-2.28.1/x86_64-w64-mingw32/include/
CHIPMUNKFLAGS =  -DOS2
AR=llvm-ar
RANLIB=llvm-ranlib
LIBX11 =  -lSDL2 -mwindows
endif
ifdef WASM
CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0 -Wno-deprecated-non-prototype -Oz
#-O1 -g
CHIPMUNKCC = emcc 
AR=emar
RANLIB=emranlib
LIBX11 =  -lSDL2 -lSDL2_gfx -lGL -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_SDL=2  -sUSE_SDL_GFX=2 -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200 -s ALLOW_MEMORY_GROWTH=1
#  -s "EXPORTED_FUNCTIONS=['_free','_malloc']" 
#  -s "EXPORTED_FUNCTIONS=['_free','_malloc']"
# --bind -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200
endif
ifdef LINUX
CHIPMUNKCC = clang -O3
#CHIPMUNKCC = x86_64-w64-mingw32-gcc  -Os -L../../SDL2-2.28.1/x86_64-w64-mingw32/lib -fomit-frame-pointer -DF_OK=0 -DLACK_GETPWNAM -D__GCC__ -I../../SDL2-2.28.1/x86_64-w64-mingw32/include/
CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0 -Wno-deprecated-non-prototype
LIBX11 =  -lSDL2 
AR=llvm-ar
RANLIB=llvm-ranlib
endif
