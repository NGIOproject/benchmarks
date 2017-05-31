CC = mpicc -lpmem
CFLAGS = -O2 -Wall -Wextra
EXE = io-bad-pmem
OBJ = $(patsubst %, %.o, $(EXE))

all: $(EXE)

io-bad-pmem: io-bad-pmem.o

io-bad-pmem.o: io-bad-pmem.c common.h Makefile

clean:
	-rm -f $(EXE) $(OBJ)
