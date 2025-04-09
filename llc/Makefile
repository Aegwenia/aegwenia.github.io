CC=gcc
CFLAGS=-Wall -Werror -pedantic -g -std=c89 -I./include

llc:
	$(CC) $(CFLAGS) -o ./build/llc.elf ./src/llc.c
clean:
	rm -f ./build/llc.elf
run: llc
	chmod +x ./build/llc.elf
	./build/llc.elf ./test/program.llc

