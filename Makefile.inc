ifdef WIN32
CHIPMUNKCC = x86_64-w64-mingw32-gcc -Os -L../../bin -fomit-frame-pointer -DLACK_GETPWNAM -D__GCC__ -I../../SDL3/include/
CHIPMUNKFLAGS =  -DOS2
AR=llvm-ar
RANLIB=llvm-ranlib
LIBX11 =  -lSDL3 -mwindows
endif
ifdef WASM
CHIPMUNKFLAGS = -DBSD -Dlinux -DF_OK=0 -Wno-deprecated-non-prototype -Oz -Wno-strict-prototypes -Wno-old-style-definition -std=gnu99 -I ../../SDL3/include
CHIPMUNKCC = emcc -sUSE_SDL=3
AR=emar
RANLIB=emranlib
LIBX11 =  -lSDL3 -lGL -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_SDL=3 -sASYNCIFY -s ASYNCIFY_STACK_SIZE=51200 -s ALLOW_MEMORY_GROWTH=1
endif
ifdef LINUX
CHIPMUNKCC = clang -O3
CHIPMUNKFLAGS = -DBSD -Dlinux -D__alpha__ -DF_OK=0 -Wno-deprecated-non-prototype -I../../SDL3/include/
LIBX11 =  -lSDL3 -Wl,-rpath,. -L../../lib/
AR=llvm-ar
RANLIB=llvm-ranlib
endif
