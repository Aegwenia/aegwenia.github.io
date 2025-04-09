CC=gcc
CFLAGS=-Wmissing-prototypes -Wall -Werror -pedantic -g -std=c89 -I./include

jik:
	$(CC) $(CFLAGS) -o ./build/jik.elf ./src/jik.c
clean:
	rm -f ./build/jik.elf
run: jik
	chmod +x ./build/jik.elf
	./build/jik.elf ./test/program.jik
