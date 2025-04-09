CC=gcc
CFLAGS=-Wmissing-prototypes -Wall -Werror -pedantic -g -std=c89 -I./include

mal:
	$(CC) $(CFLAGS) -o ./build/mal.elf ./src/mal.c
clean:
	rm -f ./build/mal.elf
run: mal
	chmod +x ./build/mal.elf
	./build/mal.elf ./test/program.mal
