CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: spaceshooter mod

spaceshooter: spaceshooter.cpp log.cpp timers.cpp djosep.cpp \
			  etapiaramire.cpp fsanchez.cpp jandrestacza.cpp \
			  jfigueroa.cpp	
	g++ $(CFLAGS) spaceshooter.cpp djosep.cpp etapiaramire.cpp \
	fsanchez.cpp jandrestacza.cpp jfigueroa.cpp log.cpp timers.cpp \
	libggfonts.a -Wall -Wextra $(LFLAGS) -o spaceshooter

mod: mod.cpp log.cpp timers.cpp djosep.cpp \
			  etapiaramire.cpp fsanchez.cpp jandrestacza.cpp \
			  jfigueroa.cpp	
	g++ $(CFLAGS) mod.cpp djosep.cpp etapiaramire.cpp \
	fsanchez.cpp jandrestacza.cpp jfigueroa.cpp log.cpp timers.cpp \
	libggfonts.a -Wall -Wextra $(LFLAGS) -o mod

clean:
	rm -f spaceshooter asteroids mod
	rm -f *.o
