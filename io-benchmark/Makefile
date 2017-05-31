CC = mpicc
CFLAGS = -O2 -Wall -Wextra
EXE = io-bad io-good
OBJ = $(patsubst %, %.o, $(EXE))

all: $(EXE)

io-bad: io-bad.o

io-good: io-good.o

io-good.o: io-good.c common.h Makefile

io-bad.o: io-bad.c common.h Makefile

clean:
	-rm -f $(EXE) $(OBJ)
