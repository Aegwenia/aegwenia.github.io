# Guide to [***MAL***](https://github.com/kanaka/mal/) C89 (`Make A Lisp`)
[***\[edit\]***](https://github.com/Aegwenia/aegwenia.github.io/edit/main/README.md)

* TOC
{:toc}

## Version 0x00 REPL

- The first version of `MAL` project using C89 standard. Responsive REPL environment.

[***Version 0x00***](./mal_00.md) [*\[source\]*](mal_00.c)

## Version 0x00 -- `text_t` and garbage collection

- Preparation for second version, `text_t` and garbage collector.

[***Version 0x00 -- 0x01***](./mal_00_text_gc.md) [*\[source\]*](mal_00_text_gc.c)


## Version 0x00 -- lists and vectors

- Adding support for basic string `(un)escaping`, new types (`list_t`/`list_p` and `vector_t`/`vector_p`), tokenizer and basic parser to the source code.

[***Version 0x00 -- 0x02***](./mal_00_list_vector.md) [*\[source\]*](mal_00_list_vector.c)


## Version 0x00 -- mal objects and errors

- Added basic support for `mal` objects and custom error handling.

[***Version 0x00***](./mal_00_mal_error.md) [*\[source\]*](mal_00_mal_error.c)

## Version 0x1

- The second version of `MAL` project using C89 standard. Reader and writer, is being written.

`gcc -Wpedantic -pedantic -Wall -Wextra -o ./mal_01 ./mal_01.c -lm`

`mal_01.c`
```C
```
