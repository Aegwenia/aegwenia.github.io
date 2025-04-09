#ifndef CORE_H
#define CORE_H
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
[+] implementation of basic string reader
*/

typedef void core_void_t, *core_void_p;
typedef char core_i8_t, *core_i8_p;
typedef unsigned char core_u8_t, *core_u8_p;
typedef short core_i16_t, *core_i16_p;
typedef unsigned short core_u16_t, *core_u16_p;
typedef int core_i32_t, *core_i32_p;
typedef unsigned int core_u32_t, *core_u32_p;
typedef long core_i64_t, *core_i64_p;
typedef unsigned long core_u64_t, *core_u64_p;
typedef float core_f32_t, *core_f32_p;
typedef double core_f64_t, *core_f64_p;

typedef enum {
  core_boolean_false,
  core_boolean_true
} core_boolean_t;

typedef enum {
  CORE_GC_TEXT
} core_gc_kind;

typedef FILE core_file_t, *core_file_p;
typedef struct core_gc_s core_gc_t, *core_gc_p;
typedef struct core_text_s core_text_t, *core_text_p;
typedef struct core_reader_s core_reader_t, *core_reader_p;
typedef struct core_lvm_s core_lvm_t, *core_lvm_p;

struct core_gc_s {
  core_gc_kind kind;
  core_u32_t mark;
  core_gc_p next;
};

struct core_text_s {
  core_gc_t gc;
  core_i8_p data;
  core_u64_t room;
  core_u64_t size;
};

struct core_reader_s {
  core_text_p source;
  core_i8_p start;
  core_i8_p current;
  core_u64_t line;
  core_u64_t column;
};

struct core_lvm_s {
  struct {
    core_u32_t mark;
    core_gc_p first;
  } gc;
  core_file_p input;
  core_file_p print;
  core_file_p whine;
  core_reader_t reader;
  core_u64_t line;
  core_u64_t column;
};

core_text_p core_text_create(core_lvm_p lvm, core_i8_p data, core_u64_t size);
core_boolean_t core_text_resize(core_lvm_p lvm, core_text_p self,
    core_u64_t size);
core_boolean_t core_text_append(core_lvm_p lvm, core_text_p self,
    core_i8_t data);
core_boolean_t core_text_extend(core_lvm_p lvm, core_text_p self,
    core_i8_p data, core_u64_t size);
core_boolean_t core_text_destroy(core_lvm_p lvm, core_text_p self);
core_void_p core_lvm_malloc(core_lvm_p lvm, core_u64_t size);
core_void_p core_lvm_calloc(core_lvm_p lvm, core_u64_t n, core_u64_t size);
core_void_p core_lvm_realloc(core_lvm_p lvm, core_void_p data, core_u64_t size);
core_void_p core_lvm_free(core_lvm_p lvm, core_void_p data);
core_boolean_t core_lvm_init(core_lvm_p lvm);
core_boolean_t core_lvm_string(core_lvm_p lvm, core_i8_p data);
core_boolean_t core_lvm_cleanup(core_lvm_p lvm);

#endif

#ifdef CORE_IMPLEMENTATION
#undef CORE_IMPLEMENTATION
#ifndef CORE_DEFINITION
#define CORE_DEFINITION
#else
#error "Multiple definitions of CORE_IMPLEMENTATION\n"
#endif

core_text_p core_text_create(core_lvm_p lvm, core_i8_p data, core_u64_t size)
{
  core_text_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "core_text_create: no lvm\n");
    return NULL;
  }
  if (!data && size) {
    fprintf(stderr, "core_text_create: no data\n");
    return NULL;
  }
  if (!(result = core_lvm_malloc(lvm, sizeof(core_text_t)))) {
    fprintf(stderr, "core_text_create: core_lvm_malloc failed\n");
    return NULL;
  }
  result->data = NULL;
  result->room = 0;
  result->size = 0;
  if (!core_text_resize(lvm, result, size)) {
    fprintf(stderr, "core_text_create: core_text_resize failed\n");
    if (core_lvm_free(lvm, result)) {
      fprintf(stderr, "core_text_create: core_lvm_free failed\n");
      return NULL;
    }
    return NULL;
  }
  result->gc.kind = CORE_GC_TEXT;
  result->gc.mark = lvm->gc.mark;
  result->gc.next = lvm->gc.first;
  lvm->gc.first = &result->gc;
  memcpy(result->data, data, size);
  result->data[size] = '\0';
  result->size = size;
  return result;
}

core_boolean_t core_text_resize(core_lvm_p lvm, core_text_p self,
    core_u64_t size)
{
  if (!lvm) {
    fprintf(stderr, "core_text_resize: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_text_resize: no self\n");
    return core_boolean_false;
  } else {
    core_i8_p data = self->data;
    core_u64_t room = self->size + size + 1;
    room = ((0 != (room & 31)) + (room >> 5)) << 5;
    if (!(data = core_lvm_realloc(lvm, data, room))) {
      fprintf(stderr, "core_text_resize: core_lvm_realloc failed\n");
      return core_boolean_false;
    }
    self->data = data;
    self->room = room;
  }
  return core_boolean_true;
}

core_boolean_t core_text_append(core_lvm_p lvm, core_text_p self,
    core_i8_t data)
{
  if (!lvm) {
    fprintf(stderr, "core_text_append: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_text_append: no self\n");
    return core_boolean_false;
  }
  if (self->room <= self->size + !!data + 1) {
    if (!core_text_resize(lvm, self, !!data)) {
      fprintf(stderr, "core_text_append: core_text_resize failed\n");
      return core_boolean_false;
    }
  }
  self->data[self->size] = data;
  if (data) {
    self->data[++self->size] = '\0';
  }
  return core_boolean_true;
}

core_boolean_t core_text_extend(core_lvm_p lvm, core_text_p self,
    core_i8_p data, core_u64_t size)
{
  if (!lvm) {
    fprintf(stderr, "core_text_extend: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_text_extend: no self\n");
    return core_boolean_false;
  }
  if (!data) {
    fprintf(stderr, "core_text_extend: no data\n");
    return core_boolean_false;
  }
  if (!size) {
    fprintf(stderr, "core_text_extend: no size\n");
    return core_boolean_false;
  }
  if (self->room <= self->size + size + 1) {
    if (!core_text_resize(lvm, self, self->size + size + 1)) {
      fprintf(stderr, "core_text_extend: core_text_resize failed\n");
      return core_boolean_false;
    }
  }
  memcpy(self->data + self->size, data, size);
  self->size += size;
  self->data[self->size] = '\0';
  return core_boolean_true;
}

core_boolean_t core_text_destroy(core_lvm_p lvm, core_text_p self)
{
  if (!lvm) {
    fprintf(stderr, "core_text_destroy: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_text_destroy: no self\n");
    return core_boolean_false;
  }
  if (!core_lvm_free(lvm, self->data)) {
    fprintf(stderr, "core_text_destroy: core_lvm_free failed\n");
    return core_boolean_false;
  }
  if (!core_lvm_free(lvm, self)) {
    fprintf(stderr, "core_text_destroy: core_lvm_free failed\n");
    return core_boolean_false;
  }
  return core_boolean_true;
}

core_boolean_t core_reader_init(core_lvm_p lvm, core_reader_p self)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_init: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_reader_init: no self\n");
    return core_boolean_false;
  }
  self->source = NULL;
  self->start = NULL;
  self->current = NULL;
  self->line = 0;
  self->column = 0;
  return core_boolean_true;
}

core_void_p core_lvm_malloc(core_lvm_p lvm, core_u64_t size)
{
  core_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "core_lvm_malloc: no lvm\n");
    return NULL;
  }
  if (!size) {
    fprintf(stderr, "core_lvm_malloc: no size\n");
    return NULL;
  }
  if (!(result = malloc(size))) {
    fprintf(stderr, "core_lvm_malloc: malloc failed\n");
    return NULL;
  }
  return result;
}

core_void_p core_lvm_calloc(core_lvm_p lvm, core_u64_t n, core_u64_t size)
{
  core_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "core_lvm_calloc: no lvm\n");
    return NULL;
  }
  if (!n) {
    fprintf(stderr, "core_lvm_calloc: no n\n");
    return NULL;
  }
  if (!size) {
    fprintf(stderr, "core_lvm_calloc: no size\n");
    return NULL;
  }
  if (!(result = calloc(n, size))) {
    fprintf(stderr, "core_lvm_calloc: calloc failed\n");
    return NULL;
  }
  return result;
}

core_void_p core_lvm_realloc(core_lvm_p lvm, core_void_p data, core_u64_t size)
{
  core_void_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "core_lvm_realloc: no lvm\n");
    return NULL;
  }
  if (!data && !size) {
    fprintf(stderr, "core_lvm_realloc: no data and size\n");
    return NULL;
  }
  if (!(result = realloc(data, size))) {
    fprintf(stderr, "core_lvm_realloc: realloc failed\n");
    return NULL;
  }
  return result;
}

core_void_p core_lvm_free(core_lvm_p lvm, core_void_p data)
{
  if (!lvm) {
    fprintf(stderr, "core_lvm_free: no lvm\n");
    return data;
  }
  if (!data) {
    fprintf(stderr, "core_lvm_free: no data\n");
    return data;
  }
  free(data);
  return NULL;
}

core_boolean_t core_lvm_init(core_lvm_p lvm)
{
  if (!lvm) {
    fprintf(stderr, "core_lvm_init: no lvm\n");
    return core_boolean_false;
  }
  lvm->gc.mark = 1;
  lvm->gc.first = NULL;
  lvm->input = stdin;
  lvm->print = stdout;
  lvm->whine = stderr;
  lvm->line = 0;
  lvm->column = 0;
  if (!core_reader_init(lvm, &lvm->reader)) {
    fprintf(stderr, "core_lvm_init: core_reader_init failed\n");
    return core_boolean_false;
  }
  return core_boolean_true;
}

core_boolean_t core_lvm_string(core_lvm_p lvm, core_i8_p data)
{
  core_text_p source = NULL;
  if (!lvm) {
    fprintf(stderr, "core_lvm_string: no lvm\n");
    return core_boolean_false;
  }
  if (!data) {
    fprintf(stderr, "core_lvm_string: no data\n");
    return core_boolean_false;
  }
  if (!(source = core_text_create(lvm, data, strlen(data)))) {
    fprintf(stderr, "core_lvm_string: core_text_create failed\n");
    return core_boolean_false;
  }
  lvm->reader.source = source;
  lvm->reader.start = source->data;
  lvm->reader.current = source->data;
  lvm->reader.line = 1;
  lvm->reader.column = 1;
  lvm->line = 1;
  lvm->column = 1;
  if (source->data && source->data[0]) {
    fprintf(lvm->print, "%s\n", source->data);
  }
  return core_boolean_true;
}

core_boolean_t core_lvm_cleanup(core_lvm_p lvm)
{
  core_boolean_t result = core_boolean_true;
  core_gc_p gc = NULL;
  if (!lvm) {
    fprintf(stderr, "core_lvm_cleanup: no lvm\n");
    return core_boolean_false;
  }
  gc = lvm->gc.first;
  while (gc) {
    core_gc_p next = gc->next;
    switch (gc->kind) {
    case CORE_GC_TEXT:
      if (!core_text_destroy(lvm, (core_text_p)gc)) {
        fprintf(stderr, "core_lvm_cleanup: core_text_destroy failed\n");
        result = core_boolean_false;
      }
      break;
    }
    gc = next;
  }
  return result;
}

#endif
