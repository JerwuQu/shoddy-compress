CC=gcc
SRC=$(wildcard *.c)
DEP=$(wildcard *.h)

shomp: $(SRC) $(DEP)
	$(CC) -Wall -Werror -std=c99 -o $@ $(SRC)

.PHONY: clean
clean:
	rm -f shomp
