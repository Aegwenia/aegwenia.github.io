<a id="top"/>

# Guide to [***MAL***](https://github.com/kanaka/mal/) C89 (`Make A Lisp`)
[***\[edit\]***](https://github.com/Aegwenia/aegwenia.github.io/edit/main/README.md)

Table of Contents (old &amp; deprecated)
- <a href="#top">Top</a>
- <a href="#v0x00-0x00">Version 0x00 -- REPL</a>
- <a href="#v0x00-0x01">Version 0x00 -- `text_t` and garbage collection</a>
- <a href="#v0x00-0x02">Version 0x00 -- lists and vectors</a>
- <a href="#v0x00-0x03">Version 0x00 -- mal objects and errors</a>
- <a href="#v0x00-0x04">Version 0x00 -- hashmap and reader macros</a>
- <a href="#v0x01-0x00">Version 0x01 -- version up</a>
- <a href="#v0x01-0x01">Version 0x01 -- environment and reworked hashmap</a>
- <a href="#v0x01-0x02">Version 0x01 -- eval and builtins</a>
- <a href="#v0x02-0x00">Version 0x02 -- version up</a>
- <a href="#v0x03-0x00">Version 0x03 -- creating and modifying environments; version up</a>
- <a href="#v0x04-0x00">Version 0x04 -- adding `if`, `fn*` and `do`; version up</a>
- <a href="#v0x05-0x00">Version 0x05 -- TCO (Tail Call Optimization); version up</a>

<a id="v0x00-0x00"/>

## Version 0x00 -- REPL

- The first version of `MAL` project using C89 standard. Responsive REPL environment.

[***Version 0x00***](./old/mal_00.md) [*\[source\]*](./old/src/mal_00.c)

<a id="v0x00-0x01"/>

### Version 0x00 -- `text_t` and garbage collection

- Preparation for second version, `text_t` and garbage collector.

[***Version 0x00 -- 0x01***](./old/mal_00_text_gc.md) [*\[source\]*](./old/src/mal_00_text_gc.c)

<a id="v0x00-0x02"/>

### Version 0x00 -- lists and vectors

- Adding support for basic string `(un)escaping`, new types (`list_t`/`list_p` and `vector_t`/`vector_p`), tokenizer and basic parser to the source code.

[***Version 0x00 -- 0x02***](./old/mal_00_list_vector.md) [*\[source\]*](./old/src/mal_00_list_vector.c)

<a id="v0x00-0x03"/>

### Version 0x00 -- mal objects and errors

- Added basic support for `mal` objects and custom error handling.

[***Version 0x00 -- 0x03***](./old/mal_00_mal_error.md) [*\[source\]*](./old/src/mal_00_mal_error.c)

<a id="v0x00-0x04"/>

### Version 0x00 -- hashmap and reader macros

- Adding support for basic hashmap, updated tokenizer in the the source code.

[***Version 0x00 -- 0x04***](./old/mal_00_hashmap.md) [*\[source\]*](./old/src/mal_00_hashmap.c)

<a id="v0x01-0x00"/>

## Version 0x01 -- version up

- The second version of `MAL` project using C89 standard. Reader, writer, lists, vectors, hashmaps ans reader macros. It is equivalent to version 0.0.4.

[***Version 0x01***](./old/mal_01.md) [*\[source\]*](./old/src/mal_01.c)

<a id="v0x01-0x01"/>

## Version 0x01 -- environment and reworked hashmap

- Added Environment, edited hashmap implementation.

[***Version 0x01 -- 0x01***](./old/mal_01_environment.md) [*\[source\]*](./old/src/mal_01_environment.c)


<a id="v0x01-0x02"/>

## Version 0x01 -- eval and builtins

Added Eval stage, and some additional buitin functions.

[***Version 0x01 -- 0x02***](./old/mal_01_eval.md) [*\[source\]*](./old/src/mal_01_eval.c)

<a id="v0x02-0x00"/>

## Version 0x02 -- version up

- The third version of `MAL` project using C89 standard. Environment, eval and some builtin functions. It is equivalent to version 0.1.2.

[***Version 0x02***](./old/mal_02.md) [*\[source\]*](./old/src/mal_02.c)

<a id="v0x03-0x00"/>

## Version 0x03 -- creating and modifying environments; version up

- The fourth version of `MAL` project using C89 standard. Creating environment using special form `let*` and modifying the current environment using `def!` special form.
 
[***Version 0x03***](./old/mal_03.md) [*\[source\]*](./old/src/mal_03.c)

<a id="v0x04-0x00"/>

## Version 0x04 -- adding `if`, `fn*` and `do`; version up

- The fifth version of `MAL` project using C89 standard. Added three new special forms `if`, `fn*` and `do` and added some core functions.
 
[***Version 0x04***](./old/mal_04.md) [*\[source\]*](./old/src/mal_04.c)

<a id="v0x05-0x00"/>

## Version 0x05 -- TCO (Tail Call Optimization); version up

- The sixth version of MAL project using C89 standard. Editing eval functions to support TCO (Tail Call Optimization).
 
[***Version 0x05***](./old/mal_05.md) [*\[source\]*](./old/src/mal_05.c)
