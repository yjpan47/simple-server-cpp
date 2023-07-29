CC=clang++
CFLAGS=-g -Wall
EXE=server

all: $(EXE)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.dSYM *.o $(EXE)

