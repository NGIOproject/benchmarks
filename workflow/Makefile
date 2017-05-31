CC=gcc -O2 -mtune=native -march=native -Wall -Wextra

SOURCES_PROD = producer.c utils.c
SOURCES_CONS = consumer.c utils.c

PROD_EXE = producer
CONS_EXE = consumer

all: $(PROD_EXE) $(CONS_EXE)

$(PROD_EXE): $(SOURCES_PROD)
	$(CC) -o $(PROD_EXE) $(SOURCES_PROD)

$(CONS_EXE): $(SOURCES_CONS)
	$(CC) -o $(CONS_EXE) $(SOURCES_CONS)

clean:
	rm -rf *~ *.o $(PROD_EXE) $(CONS_EXE)

testclean:
	rm testfile*
