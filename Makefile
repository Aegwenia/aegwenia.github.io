CC=gcc
CFLAGS =--std=c89 -Wpedantic -pedantic -Wall -Wextra -Werror -g
CLIBS=-lm
SRC=src
BIN=bin
LIB=-lm
mal: mal_00 mal_00_text_gc mal_00_list_vector mal_00_mal_error mal_00_hashmap \
	mal_01 mal_01_environment mal_01_eval mal_02 mal_03 mal_04 mal_05
	@echo `pwd`/bin/

mal_00: $(SRC)/mal_00.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_00_text_gc: $(SRC)/mal_00_text_gc.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_00_list_vector: $(SRC)/mal_00_list_vector.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_00_mal_error: $(SRC)/mal_00_mal_error.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_00_hashmap: $(SRC)/mal_00_hashmap.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_01: $(SRC)/mal_01.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_01_environment: $(SRC)/mal_01_environment.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_01_eval: $(SRC)/mal_01_eval.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_02: $(SRC)/mal_02.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_03: $(SRC)/mal_03.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_04: $(SRC)/mal_04.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
mal_05: $(SRC)/mal_05.c
	$(CC) $(CFLAGS) -o $(BIN)/$@ $< $(LIB)
.PHONY: clean
clean:
	rm $(BIN)/mal_0*
