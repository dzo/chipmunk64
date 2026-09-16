ifdef WIN32
CHIPMUNKCC = x86_64-w64-mingw32-gcc -std=gnu99 -Os
CHIPMUNKFLAGS =  -DOS2 -fomit-frame-pointer -DLACK_GETPWNAM -D__GCC__ -I../../SDL3/include/
# -D__alpha__ -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
AR=x86_64-w64-mingw32-gcc-ar
RANLIB=x86_64-w64-mingw32-gcc-ranlib
LIBX11 =  -lSDL3 -mwindows -L../../bin
endif
ifdef WASM
CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0 -Wno-deprecated-non-prototype -Oz -Wno-strict-prototypes -Wno-old-style-definition -std=gnu99 -I ../../SDL3/include
CHIPMUNKCC = emcc -sUSE_SDL=3
AR=emar
RANLIB=emranlib
LIBX11 =  -lSDL3 -lGL -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_SDL=3 -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200 -s ALLOW_MEMORY_GROWTH=1
endif
ifdef LINUX
CHIPMUNKCC = clang -std=gnu99 -Os
#  -fsanitize=address -g
CHIPMUNKFLAGS = -DBSD -Dlinux -fomit-frame-pointer -DF_OK=0 -Wno-deprecated-non-prototype -I../../SDL3/include/
LIBX11 =  -lSDL3 -Wl,-rpath,. -L../../lib/
AR=llvm-ar
RANLIB=llvm-ranlib
endif
