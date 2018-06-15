CC=gcc -fopenmp -O2 -mtune=native -march=native -Wall -Wextra  -lpmem

SOURCE = pmem-streams.c utils.c

EXE = pstreams

all: $(EXE)

$(EXE): $(SOURCE)
	$(CC) -o $(EXE) $(SOURCE)

clean:
	rm -rf *~ *.o $(EXE)

