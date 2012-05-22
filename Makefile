CC = g++-4.6

CFLAGS = -Wall -O4 -std=c++0x -mfpmath=sse -ffast-math
LDFLAGS =

PROG = grid2ply

$(PROG) : main.cpp
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean :
	rm $(PROG)


