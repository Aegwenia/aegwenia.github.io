CC=gcc
CFLAGS=-Wall -Werror -pedantic -g -std=c89 -I./include

core:
	$(CC) $(CFLAGS) -o ./build/core.elf ./src/core.c
clean:
	rm -f ./build/core.elf
run: core
	chmod +x ./build/core.elf
	./build/core.elf ./test/program.core

