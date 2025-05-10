CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic -Wconversion -fsanitize=undefined -fsanitize=address
LFLAGS=
CFILES=main.c impls.c
HFILES=*.h
EXE=./soa_test
EXE2=./aos_test
EXEFLAGS=
SELF=Makefile

all: $(EXE) $(EXE2) $(SELF)

run: all $(SELF)
	$(EXE) $(EXEFLAGS)
	$(EXE2) $(EXEFLAGS)

$(EXE): $(CFILES) $(HFILES) $(SELF)
	$(CC) $(CFLAGS) -o $(EXE) $(CFILES) $(LFLAGS)

$(EXE2): $(CFILES) $(HFILES) $(SELF)
	$(CC) $(CFLAGS) -DUSE_AOS -o $(EXE2) $(CFILES) $(LFLAGS)

clean:
	rm -f $(EXE) $(EXE2)
