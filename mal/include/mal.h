#ifndef MAL_H
#define MAL_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
[+] basic type definitions
[+] basic memory management; functions and types
[+] type for text; dynamic array
[+] type for reader; not used right now
    [+] source; text
    [+] start and current; pointer to strings for future use
    [+] line and column
[+] type for language virtual machine (lvm):
    [+] members for garbage collector
    [+] basic file streams
    [+] reader
    [+] line and column; for future use
[+] functions for text
    [+] creation
    [+] resizing
    [+] appending a character to the text with closing '\0'
    [+] extending a text by provided string
    [+] destroying a text
[+] functions for reader
    [+] initialization
[+] implementation of single-line readline
[+] implementation of repl
*/

typedef void mal_void_t, *mal_void_p;
typedef char mal_i8_t, *mal_i8_p, **mal_i8_pp;
typedef unsigned char mal_u8_t, *mal_u8_p, **mal_u8_pp;
typedef short mal_i16_t, *mal_i16_p, **mal_i16_pp;
typedef unsigned short mal_u16_t, *mal_u16_p, **mal_u16_pp;
typedef int mal_i32_t, *mal_i32_p, **mal_i32_pp;
typedef unsigned int mal_u32_t, *mal_u32_p, **mal_u32_pp;
typedef long mal_i64_t, *mal_i64_p, **mal_i64_pp;
typedef unsigned long mal_u64_t, *mal_u64_p, **mal_u64_pp;
typedef float mal_f32_t, *mal_f32_p, **mal_f32_pp;
typedef double mal_f64_t, *mal_f64_p, **mal_f64_pp;

typedef enum {
  mal_boolean_false,
  mal_boolean_true
} mal_boolean_t;

typedef enum {
  MAL_GC_TEXT
} mal_gc_kind;

typedef FILE mal_file_t, *mal_file_p;
typedef struct mal_gc_s mal_gc_t, *mal_gc_p;
typedef struct mal_text_s mal_text_t, *mal_text_p;
typedef struct mal_reader_s mal_reader_t, *mal_reader_p;
typedef struct mal_lvm_s mal_lvm_t, *mal_lvm_p;

struct mal_gc_s {
  mal_gc_kind kind;
  mal_u32_t mark;
  mal_gc_p next;
};

struct mal_text_s {
  mal_gc_t gc;
  mal_i8_p data;
  mal_u64_t room;
  mal_u64_t size;
};

struct mal_reader_s {
  mal_text_p data;
  mal_i8_p start;
  mal_i8_p current;
  mal_u64_t line;
  mal_u64_t column;
};

struct mal_lvm_s {
  struct {
    mal_gc_p first;
    mal_u32_t mark;
  } gc;
  mal_file_p input;
  mal_file_p print;
  mal_file_p whine;
  mal_reader_t reader;
  mal_u64_t line;
  mal_u64_t column;
};

mal_text_p mal_text_create(mal_lvm_p lvm, mal_i8_p data, mal_u64_t size);
mal_boolean_t mal_text_resize(mal_lvm_p lvm, mal_text_p self, mal_u64_t size);
mal_boolean_t mal_text_append(mal_lvm_p lvm, mal_text_p self, mal_i8_t c);
mal_boolean_t mal_text_extend(mal_lvm_p lvm, mal_text_p self, mal_i8_p data,
    mal_u64_t size);
mal_boolean_t mal_text_destroy(mal_lvm_p lvm, mal_text_p self);
mal_boolean_t mal_reader_init(mal_lvm_p lvm, mal_reader_p self);
mal_void_p mal_lvm_malloc(mal_lvm_p lvm, mal_u64_t size);
mal_void_p mal_lvm_calloc(mal_lvm_p lvm, mal_u64_t n, mal_u64_t size);
mal_void_p mal_lvm_realloc(mal_lvm_p lvm, mal_void_p data, mal_u64_t size);
mal_void_p mal_lvm_free(mal_lvm_p lvm, mal_void_p data);
mal_boolean_t mal_lvm_init(mal_lvm_p lvm);
mal_text_p mal_lvm_readline(mal_lvm_p lvm, mal_i8_p prompt);
mal_text_p mal_lvm_read(mal_lvm_p lvm, mal_text_p data);
mal_text_p mal_lvm_eval(mal_lvm_p lvm, mal_text_p data);
mal_text_p mal_lvm_print(mal_lvm_p lvm, mal_text_p data);
mal_text_p mal_lvm_rep(mal_lvm_p lvm, mal_text_p data);
mal_boolean_t mal_lvm_repl(mal_lvm_p lvm, mal_i8_p prompt);
mal_boolean_t mal_lvm_cleanup(mal_lvm_p lvm);

#endif

#ifdef MAL_IMPLEMENTATION
#undef MAL_IMPLEMENTATION
#ifndef MAL_DEFINITION
#define MAL_DEFINITION
#else
#error "MAL_IMPLEMENTATION already defined\n"
#endif

mal_text_p mal_text_create(mal_lvm_p lvm, mal_i8_p data, mal_u64_t size)
{
  mal_text_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_text_create: no lvm\n");
    return NULL;
  }
  if (!data && size) {
    fprintf(lvm->whine, "mal_text_create: no data\n");
    return NULL;
  }
  if (!(result = mal_lvm_malloc(lvm, sizeof(mal_text_t)))) {
    fprintf(lvm->whine, "mal_text_create: mal_lvm_malloc failed\n");
    return NULL;
  }
  result->data = NULL;
  result->room = 0;
  result->size = 0;
  if (!mal_text_resize(lvm, result, size)) {
    fprintf(lvm->whine, "mal_text_create: mal_lvm_malloc failed\n");
    if (mal_lvm_free(lvm, result)) {
      fprintf(lvm->whine, "mal_text_create: mal_lvm_free failed\n");
      return NULL;
    }
    return NULL;
  }
  result->gc.kind = MAL_GC_TEXT;
  result->gc.mark = lvm->gc.mark;
  result->gc.next = lvm->gc.first;
  lvm->gc.first = &result->gc;
  memcpy(result->data, data, size);
  result->data[size] = '\0';
  result->size = size;
  return result;
}

mal_boolean_t mal_text_resize(mal_lvm_p lvm, mal_text_p self, mal_u64_t size)
{
  if (!lvm) {
    fprintf(stderr, "mal_text_resize: no lvm\n");
    return mal_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "mal_text_resize: no self\n");
    return mal_boolean_false;
  } else {
    mal_i8_p data = self->data;
    mal_u64_t room = self->size + size + 1;
    room = ((0 != (room & 31)) + (room >> 5)) << 5;
    if (!(data = mal_lvm_realloc(lvm, data, room))) {
      fprintf(lvm->whine, "mal_text_resize: mal_lvm_realloc failed\n");
      return mal_boolean_false;
    }
    self->data = data;
    self->room = room;
  }
  return mal_boolean_true;
}

mal_boolean_t mal_text_append(mal_lvm_p lvm, mal_text_p self, mal_i8_t c)
{
  if (!lvm) {
    fprintf(stderr, "mal_text_append: no lvm\n");
    return mal_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "mal_text_append: no self\n");
    return mal_boolean_false;
  }
  if (self->room <= self->size + !!c) {
    if (!mal_text_resize(lvm, self, !!c)) {
      fprintf(lvm->whine, "mal_text_append: mal_text_resize failed\n");
      return mal_boolean_false;
    }
  }
  self->data[self->size] = c;
  if (c) {
    self->data[++self->size] = '\0';
  }
  return mal_boolean_true;
}

mal_boolean_t mal_text_extend(mal_lvm_p lvm, mal_text_p self, mal_i8_p data,
    mal_u64_t size)
{
  if (!lvm) {
    fprintf(stderr, "mal_text_extend: no lvm\n");
    return mal_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "mal_text_extend: no self\n");
    return mal_boolean_false;
  }
  if (!data) {
    fprintf(lvm->whine, "mal_text_extend: no data\n");
    return mal_boolean_false;
  }
  if (!size) {
    fprintf(lvm->whine, "mal_text_extend: no size\n");
    return mal_boolean_false;
  }
  if (self->room <= self->size + size + 1) {
    if (!mal_text_resize(lvm, self, size)) {
      fprintf(lvm->whine, "mal_text_extend: mal_text_resize failed\n");
      return mal_boolean_false;
    }
  }
  memcpy(self->data + self->size, data, size);
  self->size += size;
  self->data[self->size] = '\0';
  return mal_boolean_true;
}

mal_boolean_t mal_text_destroy(mal_lvm_p lvm, mal_text_p self)
{
  if (!lvm) {
    fprintf(stderr, "mal_text_destroy: no lvm\n");
    return mal_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "mal_text_destroy: no self\n");
    return mal_boolean_false;
  }
  if (mal_lvm_free(lvm, self->data)) {
    fprintf(lvm->whine, "mal_text_destroy: lvm_free failed\n");
    return mal_boolean_false;
  }
  if (mal_lvm_free(lvm, self)) {
    fprintf(lvm->whine, "mal_text_destroy: lvm_free failed\n");
    return mal_boolean_false;
  }
  return mal_boolean_true;
}

mal_boolean_t mal_reader_init(mal_lvm_p lvm, mal_reader_p self)
{
  if (!lvm) {
    fprintf(stderr, "mal_reader_init: no lvm\n");
    return mal_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "mal_reader_init: no self\n");
    return mal_boolean_false;
  }
  self->data = NULL;
  self->start = NULL;
  self->current = NULL;
  self->line = 0;
  self->column = 0;
  return mal_boolean_true;
}

mal_void_p mal_lvm_malloc(mal_lvm_p lvm, mal_u64_t size)
{
  mal_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_lvm_malloc: no lvm\n");
    return NULL;
  }
  if (!size) {
    fprintf(lvm->whine, "mal_lvm_malloc: no size\n");
    return NULL;
  }
  if (!(result = malloc(size))) {
    fprintf(lvm->whine, "mal_lvm_malloc: malloc failed\n");
    return NULL;
  }
  return result;
}

mal_void_p mal_lvm_calloc(mal_lvm_p lvm, mal_u64_t n, mal_u64_t size)
{
  mal_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_lvm_calloc: no lvm\n");
    return NULL;
  }
  if (!n) {
    fprintf(lvm->whine, "mal_lvm_calloc: no n\n");
    return NULL;
  }
  if (!size) {
    fprintf(lvm->whine, "mal_lvm_calloc: no size\n");
    return NULL;
  }
  if (!(result = calloc(n, size))) {
    fprintf(lvm->whine, "mal_lvm_calloc: malloc failed\n");
    return NULL;
  }
  return result;
}

mal_void_p mal_lvm_realloc(mal_lvm_p lvm, mal_void_p data, mal_u64_t size)
{
  mal_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_lvm_realloc: no lvm\n");
    return NULL;
  }
  if (!data && !size) {
    fprintf(lvm->whine, "mal_lvm_realloc: no data and size\n");
    return NULL;
  }
  if (!(result = realloc(data, size)) && size) {
    fprintf(lvm->whine, "mal_lvm_malloc: realloc failed\n");
    return NULL;
  }
  return result;
}

mal_void_p mal_lvm_free(mal_lvm_p lvm, mal_void_p data)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_free: no lvm\n");
    return data;
  }
  if (!data) {
    fprintf(lvm->whine, "mal_lvm_free: no data\n");
    return data;
  }
  free(data);
  return NULL;
}

mal_boolean_t mal_lvm_init(mal_lvm_p lvm)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_init: no lvm\n");
    return mal_boolean_false;
  }
  lvm->gc.mark = 1;
  lvm->gc.first = NULL;
  lvm->input = stdin;
  lvm->print = stdout;
  lvm->whine = stderr;
  lvm->line = 0;
  lvm->column = 0;
  if (!mal_reader_init(lvm, &lvm->reader)) {
    fprintf(lvm->whine, "mal_lvm_init: mal_reader_init failed\n");
    return mal_boolean_false;
  }
  return mal_boolean_true;
}

mal_text_p mal_lvm_readline(mal_lvm_p lvm, mal_i8_p prompt)
{
  mal_text_p result = NULL;
  mal_i8_t buffer[32] = {'\0'};
  if (!lvm) {
    fprintf(stderr, "mal_lvm_readline: no lvm\n");
    return NULL;
  }
  if (!(result = mal_text_create(lvm, NULL, 0))) {
    fprintf(lvm->whine, "mal_lvm_readline: mal_text_create failed\n");
    return NULL;
  }
  if (!prompt) {
    prompt = "mal";
  }
  lvm->reader.data = result;
  lvm->reader.start = result->data;
  lvm->reader.current = result->data;
  lvm->reader.line = 1;
  lvm->reader.column = 1;
  lvm->line = 1;
  lvm->column = 1;
  fprintf(lvm->print, "%s> ", prompt);
  while (fgets(buffer, 32, lvm->input)) {
    mal_boolean_t quit = mal_boolean_false;
    mal_u32_t size = 0;
    size = strcspn(buffer, "\r\n");
    if (!size) {
      return result;
    }
    if (31 != size && '\n' != buffer[size - 1]) {
      quit = mal_boolean_true;
    }
    buffer[size] = '\0';
    if (!(mal_text_extend(lvm, result, buffer, size))) {
      fprintf(lvm->whine, "mal_lvm_readline: mal_text_append failed\n");
      return NULL;
    }
    if (quit) {
      return result;
    }
  }
  return result;
}

mal_text_p mal_lvm_read(mal_lvm_p lvm, mal_text_p source)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_read: no lvm\n");
    return NULL;
  }
  if (!source) {
    fprintf(lvm->whine, "mal_lvm_read: no data\n");
    return NULL;
  }
  return source;
}

mal_text_p mal_lvm_eval(mal_lvm_p lvm, mal_text_p ast)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_eval: no lvm\n");
    return NULL;
  }
  if (!ast) {
    fprintf(lvm->whine, "mal_lvm_eval: no ast\n");
    return NULL;
  }
  return ast;
}

mal_text_p mal_lvm_print(mal_lvm_p lvm, mal_text_p ast)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_print: no lvm\n");
    return NULL;
  }
  if (!ast) {
    fprintf(lvm->whine, "mal_lvm_print: no ast\n");
    return NULL;
  }
  if (ast->data && ast->data[0]) {
    fprintf(lvm->print, "%s\n", ast->data);
  }
  return ast;
}

mal_text_p mal_lvm_rep(mal_lvm_p lvm, mal_text_p data)
{
  mal_text_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_lvm_rep: no lvm\n");
    return NULL;
  }
  if (!data) {
    fprintf(lvm->whine, "mal_lvm_rep: no data\n");
    return NULL;
  }
  if (!(result = mal_lvm_read(lvm, data))) {
    fprintf(lvm->whine, "mal_lvm_rep: mal_lvm_read failed\n");
    return NULL;
  }
  if (!(result = mal_lvm_eval(lvm, result))) {
    fprintf(lvm->whine, "mal_lvm_rep: mal_lvm_eval failed\n");
    return NULL;
  }
  if (!(result = mal_lvm_print(lvm, result))) {
    fprintf(lvm->whine, "mal_lvm_rep: mal_lvm_print failed\n");
    return NULL;
  }
  return result;
}

mal_boolean_t mal_lvm_repl(mal_lvm_p lvm, mal_i8_p prompt)
{
  if (!lvm) {
    fprintf(stderr, "mal_lvm_repl: no lvm\n");
    return mal_boolean_false;
  }
  fputs("Make-a-Lisp version 0.0.0\n", lvm->print);
  fputs("Type (exit) or press Ctrl+D to exit\n", lvm->print);
  while (1) {
    mal_text_p result = NULL;
    if (!(result = mal_lvm_readline(lvm, prompt))) {
      fprintf(lvm->whine, "mal_lvm_repl: mal_lvm_readline failed\n");
      return mal_boolean_false;
    }
    if (feof(lvm->input) || 0 == strncmp("(exit)", result->data, 6)) {
      break;
    }
    if (!(mal_lvm_rep(lvm, result))) {
      fprintf(lvm->whine, "mal_lvm_repl: mal_lvm_rep failed\n");
      return mal_boolean_false;
    }
  }
  return mal_boolean_true;
}

mal_boolean_t mal_lvm_cleanup(mal_lvm_p lvm)
{
  mal_boolean_t result = mal_boolean_true;
  mal_gc_p gc = NULL;
  if (!lvm) {
    fprintf(stderr, "mal_lvm_cleanup: no lvm\n");
    return mal_boolean_false;
  }
  gc = lvm->gc.first;
  while (gc) {
    mal_gc_p next = gc->next;
    switch (gc->kind) {
    case MAL_GC_TEXT:
      if (!mal_text_destroy(lvm, (mal_text_p)gc)) {
        fprintf(lvm->whine, "mal_lvm_cleanup: mal_text_destroy failed\n");
        result = mal_boolean_false;
      }
      break;
    }
    gc = next;
  }
  return result;
}
#endif

