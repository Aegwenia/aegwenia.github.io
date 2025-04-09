#ifndef CORE_H
#define CORE_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
[+] type for tokens and token kinds
[~] gc type kind added for token
[~] added some members to reader
[+] functions for tokens
    [+] creation of valid token
    [+] creation of invalid token
    [+] geting string representation of token kind
    [+] printing representation of token
    [+] destroying a token
[+] functions for reader
    [~] edited initialisation
    [+] storing source and additional members; previously in initialisation
    [+] peeking a (next) character
    [+] advancing and returning a (next) character
    [+] shifting to the next token reading
    [+] consuming whitespace and making tokens in background
    [+] checking for a prefix for reading a nested comments
    [+] skipping whitespace and comments
    [+] reading a number token
    [+] reading an identifier token
    [+] reading a character token
    [+] reading a string token
    [+] scanning for the next token
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
  CORE_GC_TEXT,
  CORE_GC_TOKEN
} core_gc_kind;

typedef enum {
  CORE_TOKEN_INVALID,
  CORE_TOKEN_UNKNOWN,
  CORE_TOKEN_CHARACTER,
  CORE_TOKEN_INTEGER,
  CORE_TOKEN_DECIMAL,
  CORE_TOKEN_IDENTIFIER,
  CORE_TOKEN_STRING,
  CORE_TOKEN_COMMENT,
  CORE_TOKEN_NL,
  CORE_TOKEN_CR,
  CORE_TOKEN_SPACE,
  CORE_TOKEN_TAB,
  CORE_TOKEN_FF,
  CORE_TOKEN_RANGE, /* .. */
  CORE_TOKEN_ELLIPSIS, /* ... */
  CORE_TOKEN_HASH, /* # */
  CORE_TOKEN_CONCAT, /* ## */
  CORE_TOKEN_PLUS, /* + */
  CORE_TOKEN_MINUS, /* - */
  CORE_TOKEN_ASTERISK, /* * */
  CORE_TOKEN_SLASH, /* / */
  CORE_TOKEN_PERCENT, /* % */
  CORE_TOKEN_CARET, /* ^ */
  CORE_TOKEN_AMPERSAND, /* & */
  CORE_TOKEN_PIPE, /* | */
  CORE_TOKEN_TILDE, /* ~ */
  CORE_TOKEN_EXCLAMATION, /* ! */
  CORE_TOKEN_QUESTION, /* ? */
  CORE_TOKEN_COLON, /* : */
  CORE_TOKEN_ASSIGNMENT, /* = */
  CORE_TOKEN_PLUSA, /* += */
  CORE_TOKEN_MINUSA, /* -= */
  CORE_TOKEN_ASTERISKA, /* *= */
  CORE_TOKEN_SLASHA, /* /= */
  CORE_TOKEN_PERCENTA, /* %= */
  CORE_TOKEN_AMPERSANDA, /* &= */
  CORE_TOKEN_PIPEA, /* |= */
  CORE_TOKEN_CARETA, /* ^= */
  CORE_TOKEN_LSHIFT, /* << */
  CORE_TOKEN_RSHIFT, /* >> */
  CORE_TOKEN_LSHIFTA, /* <<= */
  CORE_TOKEN_RSHIFTA, /* >>= */
  CORE_TOKEN_LT, /* < */
  CORE_TOKEN_EQ, /* == */
  CORE_TOKEN_GT, /* > */
  CORE_TOKEN_GE, /* >= */
  CORE_TOKEN_NE, /* != */
  CORE_TOKEN_LE, /* <= */
  CORE_TOKEN_LOGAND, /* && */
  CORE_TOKEN_LOGOR, /* || */
  CORE_TOKEN_INCREMENT, /* ++ */
  CORE_TOKEN_DECREMENT, /* -- */
  CORE_TOKEN_SEMICOLON, /* ; */
  CORE_TOKEN_COMMA, /* , */
  CORE_TOKEN_DOT, /* . */
  CORE_TOKEN_ARROW, /* -> */
  CORE_TOKEN_LPAREN, /* ( */
  CORE_TOKEN_RPAREN, /* ) */
  CORE_TOKEN_LBRACKET, /* [ */
  CORE_TOKEN_RBRACKET, /* ] */
  CORE_TOKEN_LBRACE, /* { */
  CORE_TOKEN_RBRACE, /* } */
  CORE_TOKEN_KIF, /* if */
  CORE_TOKEN_KELSE, /* else */
  CORE_TOKEN_KWHILE, /* while */
  CORE_TOKEN_KDO, /* do */
  CORE_TOKEN_KFOR, /* for */
  CORE_TOKEN_KBREAK, /* break */
  CORE_TOKEN_KCONTINUE, /* continue */
  CORE_TOKEN_KRETURN, /* return */
  CORE_TOKEN_KSWITCH, /* switch */
  CORE_TOKEN_KCASE, /* case */
  CORE_TOKEN_KDEFAULT, /* default */
  CORE_TOKEN_KGOTO, /* goto */
  CORE_TOKEN_KSIZEOF, /* sizeof */
  CORE_TOKEN_KTYPEDEF, /* typedef */
  CORE_TOKEN_KSTRUCT, /* struct */
  CORE_TOKEN_KUNION, /* union */
  CORE_TOKEN_KENUM, /* enum */
  CORE_TOKEN_KCONST, /* const */
  CORE_TOKEN_KVOLATILE, /* volatile */
  CORE_TOKEN_KAUTO, /* auto */
  CORE_TOKEN_KREGISTER, /* register */
  CORE_TOKEN_KSTATIC, /* static */
  CORE_TOKEN_KEXTERN, /* extern */
  CORE_TOKEN_KINLINE, /* inline */
  CORE_TOKEN_KVOID, /* void */
  CORE_TOKEN_KI8, /* i8_t */
  CORE_TOKEN_KU8, /* u8_t */
  CORE_TOKEN_KI16, /* i16_t */
  CORE_TOKEN_KU16, /* u16_t */
  CORE_TOKEN_KI32, /* i32_t */
  CORE_TOKEN_KU32, /* u32_t */
  CORE_TOKEN_KI64, /* i64_t */
  CORE_TOKEN_KU64, /* u64_t */
  CORE_TOKEN_KF32, /* f32_t */
  CORE_TOKEN_KF64, /* f64_t */
  CORE_TOKEN_KC32, /* c32_t */
  CORE_TOKEN_KC64, /* c64_t */
  CORE_TOKEN_KSTRING, /* string_t */
  CORE_TOKEN_EOI,
  CORE_TOKEN_COUNT
} core_token_kind;

typedef FILE core_file_t, *core_file_p;
typedef struct core_gc_s core_gc_t, *core_gc_p;
typedef struct core_text_s core_text_t, *core_text_p;
typedef struct core_token_s core_token_t, *core_token_p;
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

struct core_token_s {
  core_gc_t gc;
  core_token_kind kind;
  core_text_p text;
  core_token_p next;
  core_u64_t line;
  core_u64_t column;
};

struct core_reader_s {
  core_text_p source;
  core_i8_p start;
  core_i8_p current;
  core_u64_t line;
  core_u64_t column;
  core_boolean_t ready;
  core_boolean_t nl;
  core_u32_t nested;
  core_token_p first;
  core_token_p last;
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
core_token_p core_token_create(core_lvm_p lvm, core_token_kind kind);
core_token_p core_token_invalid(core_lvm_p lvm, core_i8_p data,
    core_u64_t size);
core_i8_p core_token_name(core_lvm_p lvm, core_token_kind kind);
core_boolean_t core_token_print(core_lvm_p lvm, core_token_p token);
core_boolean_t core_token_destroy(core_lvm_p lvm, core_token_p token);
core_boolean_t core_reader_init(core_lvm_p lvm, core_reader_p self);
core_boolean_t core_reader_source(core_lvm_p lvm, core_reader_p self,
    core_text_p source);
core_i8_t core_reader_peek(core_lvm_p lvm, core_reader_p self,
    core_boolean_t next);
core_i8_t core_reader_advance(core_lvm_p lvm, core_reader_p self,
		core_boolean_t next);
core_boolean_t core_reader_shift(core_lvm_p lvm, core_reader_p self);
core_i8_t core_reader_consume(core_lvm_p lvm, core_reader_p self, core_i8_t c);
core_boolean_t core_reader_prefix(core_lvm_p lvm, core_reader_p self,
    core_i8_p prefix);
core_i8_t core_reader_whitespace(core_lvm_p lvm, core_reader_p self);
core_token_p core_reader_number(core_lvm_p lvm, core_reader_p self);
core_token_p core_reader_identifier(core_lvm_p lvm, core_reader_p self);
core_token_p core_reader_character(core_lvm_p lvm, core_reader_p self);
core_token_p core_reader_string(core_lvm_p lvm, core_reader_p self);
core_token_p core_reader_scan(core_lvm_p lvm, core_reader_p self);
core_void_p core_lvm_malloc(core_lvm_p lvm, core_u64_t size);
core_void_p core_lvm_calloc(core_lvm_p lvm, core_u64_t n, core_u64_t size);
core_void_p core_lvm_realloc(core_lvm_p lvm, core_void_p data, core_u64_t size);
core_void_p core_lvm_free(core_lvm_p lvm, core_void_p data);
core_boolean_t core_lvm_init(core_lvm_p lvm);
core_reader_p core_lvm_reader(core_lvm_p lvm);
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
  if (core_lvm_free(lvm, self->data)) {
    fprintf(stderr, "core_text_destroy: core_lvm_free failed\n");
    return core_boolean_false;
  }
  if (core_lvm_free(lvm, self)) {
    fprintf(stderr, "core_text_destroy: core_lvm_free failed\n");
    return core_boolean_false;
  }
  return core_boolean_true;
}

core_token_p core_token_create(core_lvm_p lvm, core_token_kind kind)
{
  core_token_p result = NULL;
  core_reader_p reader = NULL;
  core_u64_t size = 0;
  if (!lvm) {
    fprintf(stderr, "core_token_create: no lvm\n");
    return NULL;
  }
  if (!(result = core_lvm_calloc(lvm, 1, sizeof(core_token_t)))) {
    fprintf(lvm->whine, "core_token_create: core_lvm_calloc failed\n");
    return NULL;
  }
  result->gc.kind = CORE_GC_TOKEN;
  result->gc.mark = lvm->gc.mark;
  result->gc.next = lvm->gc.first;
  lvm->gc.first = &result->gc;
  result->kind = kind;
  result->line = lvm->line;
  result->column = lvm->column;
  result->next = NULL;
  if (!(reader = core_lvm_reader(lvm))) {
    fprintf(lvm->whine, "core_token_create: core_lvm_reader failed\n");
    return NULL;
  }
  if (!reader->first) {
    reader->first = result;
    reader->last = result;
  } else {
    reader->last->next = result;
    reader->last = result;
  }
  switch (kind) {
  case CORE_TOKEN_INVALID:
    break;
  case CORE_TOKEN_CHARACTER:
  case CORE_TOKEN_STRING:
    size = reader->current - reader->start - 2;
    if (!(result->text = core_text_create(lvm, reader->start + 1, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_INTEGER:
  case CORE_TOKEN_DECIMAL:
  case CORE_TOKEN_IDENTIFIER:
  case CORE_TOKEN_COMMENT:
  case CORE_TOKEN_NL:
  case CORE_TOKEN_CR:
  case CORE_TOKEN_SPACE:
  case CORE_TOKEN_TAB:
  case CORE_TOKEN_FF:
    size = reader->current - reader->start;
    if (!(result->text = core_text_create(lvm, reader->start, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_PLUS: /* + */
  case CORE_TOKEN_MINUS: /* - */
  case CORE_TOKEN_ASTERISK: /* * */
  case CORE_TOKEN_SLASH: /* / */
  case CORE_TOKEN_PERCENT: /* % */
  case CORE_TOKEN_CARET: /* ^ */
  case CORE_TOKEN_AMPERSAND: /* & */
  case CORE_TOKEN_PIPE: /* | */
  case CORE_TOKEN_TILDE: /* ~ */
  case CORE_TOKEN_EXCLAMATION: /* ! */
  case CORE_TOKEN_QUESTION: /* ? */
  case CORE_TOKEN_COLON: /* : */
  case CORE_TOKEN_ASSIGNMENT: /* = */
  case CORE_TOKEN_LT: /* < */
  case CORE_TOKEN_GT: /* > */
  case CORE_TOKEN_SEMICOLON: /* ; */
  case CORE_TOKEN_COMMA: /* , */
  case CORE_TOKEN_DOT: /* . */
  case CORE_TOKEN_LPAREN: /* ( */
  case CORE_TOKEN_RPAREN: /* ) */
  case CORE_TOKEN_LBRACKET: /* [ */
  case CORE_TOKEN_RBRACKET: /* ] */
  case CORE_TOKEN_LBRACE: /* { */
  case CORE_TOKEN_RBRACE: /* } */
  case CORE_TOKEN_HASH: /* # */
    size = reader->current - reader->start;
    if (1 != size) {
      fprintf(lvm->whine, "core_token_create: 1 != size '%.*s'\n",
          (core_i8_t)size, reader->start);
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, reader->start, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_PLUSA: /* += */
  case CORE_TOKEN_MINUSA: /* -= */
  case CORE_TOKEN_ASTERISKA: /* *= */
  case CORE_TOKEN_SLASHA: /* /= */
  case CORE_TOKEN_PERCENTA: /* %= */
  case CORE_TOKEN_AMPERSANDA: /* &= */
  case CORE_TOKEN_PIPEA: /* |= */
  case CORE_TOKEN_CARETA: /* != */
  case CORE_TOKEN_LSHIFT: /* << */
  case CORE_TOKEN_RSHIFT: /* >> */
  case CORE_TOKEN_EQ: /* == */
  case CORE_TOKEN_GE: /* >= */
  case CORE_TOKEN_NE: /* != */
  case CORE_TOKEN_LE: /* <= */
  case CORE_TOKEN_LOGAND: /* && */
  case CORE_TOKEN_LOGOR: /* || */
  case CORE_TOKEN_INCREMENT: /* ++ */
  case CORE_TOKEN_DECREMENT: /* -- */
  case CORE_TOKEN_ARROW: /* -> */
  case CORE_TOKEN_CONCAT: /* ## */
  case CORE_TOKEN_RANGE: /* .. */
    size = reader->current - reader->start;
    if (2 != size) {
      fprintf(lvm->whine, "core_token_create: 2 != size '%.*s'\n",
          (core_i8_t)size, reader->start);
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, reader->start, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_LSHIFTA: /* <<= */
  case CORE_TOKEN_RSHIFTA: /* >>= */
  case CORE_TOKEN_ELLIPSIS: /* ... */
    size = reader->current - reader->start;
    if (3 != size) {
      fprintf(lvm->whine, "core_token_create: 3 != size '%.*s'\n",
          (core_i8_t)size, reader->start);
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, reader->start, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_KIF: /* if */
  case CORE_TOKEN_KELSE: /* else */
  case CORE_TOKEN_KWHILE: /* while */
  case CORE_TOKEN_KDO: /* do */
  case CORE_TOKEN_KFOR: /* for */
  case CORE_TOKEN_KBREAK: /* break */
  case CORE_TOKEN_KCONTINUE: /* continue */
  case CORE_TOKEN_KRETURN: /* return */
  case CORE_TOKEN_KSWITCH: /* switch */
  case CORE_TOKEN_KCASE: /* case */
  case CORE_TOKEN_KDEFAULT: /* default */
  case CORE_TOKEN_KGOTO: /* goto */
  case CORE_TOKEN_KSIZEOF: /* sizeof */
  case CORE_TOKEN_KTYPEDEF: /* typedef */
  case CORE_TOKEN_KSTRUCT: /* struct */
  case CORE_TOKEN_KUNION: /* union */
  case CORE_TOKEN_KENUM: /* enum */
  case CORE_TOKEN_KCONST: /* const */
  case CORE_TOKEN_KVOLATILE: /* volatile */
  case CORE_TOKEN_KAUTO: /* auto */
  case CORE_TOKEN_KREGISTER: /* register */
  case CORE_TOKEN_KSTATIC: /* static */
  case CORE_TOKEN_KEXTERN: /* extern */
  case CORE_TOKEN_KINLINE: /* inline */
  case CORE_TOKEN_KVOID: /* void */
  case CORE_TOKEN_KI8: /* i8_t */
  case CORE_TOKEN_KU8: /* u8_t */
  case CORE_TOKEN_KI16: /* i16_t */
  case CORE_TOKEN_KU16: /* u16_t */
  case CORE_TOKEN_KI32: /* i32_t */
  case CORE_TOKEN_KU32: /* u32_t */
  case CORE_TOKEN_KI64: /* i64_t */
  case CORE_TOKEN_KU64: /* u64_t */
  case CORE_TOKEN_KF32: /* f32_t */
  case CORE_TOKEN_KF64: /* f64_t */
  case CORE_TOKEN_KC32: /* c32_t */
  case CORE_TOKEN_KC64: /* c64_t */
  case CORE_TOKEN_KSTRING: /* string_t */
  case CORE_TOKEN_UNKNOWN:
    size = reader->current - reader->start;
    if (!(result->text = core_text_create(lvm, reader->start, size))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  case CORE_TOKEN_EOI: /* end of input */
    if (!(result->text = core_text_create(lvm, NULL, 0))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    break;
  default:
    fprintf(lvm->whine, "core_token_create: unknown kind '%s'\n",
        core_token_name(lvm, kind));
    if (!(result->text = core_text_create(lvm, NULL, 0))) {
      fprintf(lvm->whine, "core_token_create: core_text_create failed\n");
      return NULL;
    }
    return NULL;
  }
  return result;
}

core_token_p core_token_invalid(core_lvm_p lvm, core_i8_p data, core_u64_t size)
{
  core_token_p result = NULL;
  if (!lvm) {
    fprintf(stderr, "core_token_invalid: no lvm\n");
    return NULL;
  }
  if (!data) {
    fprintf(lvm->whine, "core_token_invalid: no data\n");
    return NULL;
  }
  if (!(result = core_token_create(lvm, CORE_TOKEN_INVALID))) {
    fprintf(lvm->whine, "core_token_invalid: core_token_create failed\n");
    return NULL;
  }
  if (!(result->text = core_text_create(lvm, data, size))) {
    fprintf(lvm->whine, "core_token_invalid: core_text_create failed\n");
    return NULL;
  }
  return result;
}

core_i8_p core_token_name(core_lvm_p lvm, core_token_kind kind)
{
  if (!lvm) {
    fprintf(stderr, "core_token_name: no lvm\n");
    return "CORE_TOKEN_UNKNOWN";
  } else {
    switch (kind) {
    case CORE_TOKEN_INVALID:
      return "CORE_TOKEN_INVALID";
    case CORE_TOKEN_CHARACTER:
      return "CORE_TOKEN_CHARACTER";
    case CORE_TOKEN_INTEGER:
      return "CORE_TOKEN_INTEGER";
    case CORE_TOKEN_DECIMAL:
      return "CORE_TOKEN_DECIMAL";
    case CORE_TOKEN_IDENTIFIER:
      return "CORE_TOKEN_IDENTIFIER";
    case CORE_TOKEN_STRING:
      return "CORE_TOKEN_STRING";
    case CORE_TOKEN_COMMENT:
      return "CORE_TOKEN_COMMENT";
    case CORE_TOKEN_NL:
      return "CORE_TOKEN_NL";
    case CORE_TOKEN_CR:
      return "CORE_TOKEN_CR";
    case CORE_TOKEN_SPACE:
      return "CORE_TOKEN_SPACE";
    case CORE_TOKEN_TAB:
      return "CORE_TOKEN_TAB";
    case CORE_TOKEN_FF:
      return "CORE_TOKEN_FF";
    case CORE_TOKEN_RANGE: /* .. */
      return "CORE_TOKEN_RANGE";
    case CORE_TOKEN_ELLIPSIS: /* ... */
      return "CORE_TOKEN_ELLIPSIS";
    case CORE_TOKEN_HASH:
      return "CORE_TOKEN_HASH"; /* # */
    case CORE_TOKEN_CONCAT: /* ## */
      return "CORE_TOKEN_CONCAT";
    case CORE_TOKEN_PLUS: /* + */
      return "CORE_TOKEN_PLUS";
    case CORE_TOKEN_MINUS: /* - */
      return "CORE_TOKEN_MINUS";
    case CORE_TOKEN_ASTERISK: /* * */
      return "CORE_TOKEN_ASTERISK";
    case CORE_TOKEN_SLASH: /* / */
      return "CORE_TOKEN_SLASH";
    case CORE_TOKEN_PERCENT: /* % */
      return "CORE_TOKEN_PERCENT";
    case CORE_TOKEN_CARET: /* ^ */
      return "CORE_TOKEN_CARET";
    case CORE_TOKEN_AMPERSAND: /* & */
      return "CORE_TOKEN_AMPERSAND";
    case CORE_TOKEN_PIPE: /* | */
      return "CORE_TOKEN_PIPE";
    case CORE_TOKEN_TILDE: /* ~ */
      return "CORE_TOKEN_TILDE";
    case CORE_TOKEN_EXCLAMATION: /* ! */
      return "CORE_TOKEN_EXCLAMATION";
    case CORE_TOKEN_QUESTION: /* ? */
      return "CORE_TOKEN_QUESTION";
    case CORE_TOKEN_COLON: /* : */
      return "CORE_TOKEN_COLON";
    case CORE_TOKEN_ASSIGNMENT: /* = */
      return "CORE_TOKEN_ASSIGNMENT";
    case CORE_TOKEN_PLUSA: /* += */
      return "CORE_TOKEN_PLUSA";
    case CORE_TOKEN_MINUSA: /* -= */
      return "CORE_TOKEN_MINUSA";
    case CORE_TOKEN_ASTERISKA: /* *= */
      return "CORE_TOKEN_ASTERISKA";
    case CORE_TOKEN_SLASHA: /* /= */
      return "CORE_TOKEN_SLASHA";
    case CORE_TOKEN_PERCENTA: /* %= */
      return "CORE_TOKEN_PERCENTA";
    case CORE_TOKEN_AMPERSANDA: /* &= */
      return "CORE_TOKEN_AMPERSANDA";
    case CORE_TOKEN_PIPEA: /* |= */
      return "CORE_TOKEN_PIPEA";
    case CORE_TOKEN_CARETA: /* != */
      return "CORE_TOKEN_CARETA";
    case CORE_TOKEN_LSHIFT: /* << */
      return "CORE_TOKEN_LSHIFT";
    case CORE_TOKEN_RSHIFT: /* >> */
      return "CORE_TOKEN_RSHIFT";
    case CORE_TOKEN_LSHIFTA: /* <<= */
      return "CORE_TOKEN_LSHIFTA";
    case CORE_TOKEN_RSHIFTA: /* >>= */
      return "CORE_TOKEN_RSHIFTA";
    case CORE_TOKEN_LT: /* < */
      return "CORE_TOKEN_LT";
    case CORE_TOKEN_EQ: /* == */
      return "CORE_TOKEN_EQ";
    case CORE_TOKEN_GT: /* > */
      return "CORE_TOKEN_GT";
    case CORE_TOKEN_GE: /* >= */
      return "CORE_TOKEN_GE";
    case CORE_TOKEN_NE: /* != */
      return "CORE_TOKEN_NE";
    case CORE_TOKEN_LE: /* <= */
      return "CORE_TOKEN_LE";
    case CORE_TOKEN_LOGAND: /* && */
      return "CORE_TOKEN_LOGAND";
    case CORE_TOKEN_LOGOR: /* || */
      return "CORE_TOKEN_LOGOR";
    case CORE_TOKEN_INCREMENT: /* ++ */
      return "CORE_TOKEN_INCREMENT";
    case CORE_TOKEN_DECREMENT: /* -- */
      return "CORE_TOKEN_DECREMENT";
    case CORE_TOKEN_SEMICOLON: /* ; */
      return "CORE_TOKEN_SEMICOLON";
    case CORE_TOKEN_COMMA: /* : */
      return "CORE_TOKEN_COMMA";
    case CORE_TOKEN_DOT: /* . */
      return "CORE_TOKEN_DOT";
    case CORE_TOKEN_ARROW: /* -> */
      return "CORE_TOKEN_ARROW";
    case CORE_TOKEN_LPAREN: /* ( */
      return "CORE_TOKEN_LPAREN";
    case CORE_TOKEN_RPAREN: /* ) */
      return "CORE_TOKEN_RPAREN";
    case CORE_TOKEN_LBRACKET: /* [ */
      return "CORE_TOKEN_LBRACKET";
    case CORE_TOKEN_RBRACKET: /* ] */
      return "CORE_TOKEN_RBRACKET";
    case CORE_TOKEN_LBRACE: /* { */
      return "CORE_TOKEN_LBRACE";
    case CORE_TOKEN_RBRACE: /* } */
      return "CORE_TOKEN_RBRACE";
    case CORE_TOKEN_KIF: /* if */
      return "CORE_TOKEN_KIF";
    case CORE_TOKEN_KELSE: /* else */
      return "CORE_TOKEN_KELSE";
    case CORE_TOKEN_KWHILE: /* while */
      return "CORE_TOKEN_KWHILE";
    case CORE_TOKEN_KDO: /* do */
      return "CORE_TOKEN_KDO";
    case CORE_TOKEN_KFOR: /* for */
      return "CORE_TOKEN_KFOR";
      return "CORE_TOKEN_KBREAK";
    case CORE_TOKEN_KCONTINUE: /* continue */
      return "CORE_TOKEN_KCONTINUE";
    case CORE_TOKEN_KRETURN: /* return */
      return "CORE_TOKEN_KRETURN";
    case CORE_TOKEN_KSWITCH: /* switch */
      return "CORE_TOKEN_KSWITCH";
    case CORE_TOKEN_KCASE: /* case */
      return "CORE_TOKEN_KCASE";
    case CORE_TOKEN_KDEFAULT: /* default */
      return "CORE_TOKEN_KDEFAULT";
    case CORE_TOKEN_KGOTO: /* goto */
      return "CORE_TOKEN_KGOTO";
    case CORE_TOKEN_KSIZEOF: /* sizeof */
      return "CORE_TOKEN_KSIZEOF";
    case CORE_TOKEN_KTYPEDEF: /* typedef */
      return "CORE_TOKEN_KTYPEDEF";
    case CORE_TOKEN_KSTRUCT: /* struct */
      return "CORE_TOKEN_KSTRUCT";
    case CORE_TOKEN_KUNION: /* union */
      return "CORE_TOKEN_KUNION";
    case CORE_TOKEN_KENUM: /* enum */
      return "CORE_TOKEN_KENUM";
    case CORE_TOKEN_KCONST: /* const */
      return "CORE_TOKEN_KCONST";
    case CORE_TOKEN_KVOLATILE: /* volatile */
      return "CORE_TOKEN_KVOLATILE";
    case CORE_TOKEN_KAUTO: /* auto */
      return "CORE_TOKEN_KAUTO";
    case CORE_TOKEN_KREGISTER: /* register */
      return "CORE_TOKEN_KREGISTER";
    case CORE_TOKEN_KSTATIC: /* static */
      return "CORE_TOKEN_KSTATIC";
    case CORE_TOKEN_KEXTERN: /* extern */
      return "CORE_TOKEN_KEXTERN";
    case CORE_TOKEN_KINLINE: /* inline */
      return "CORE_TOKEN_KINLINE";
    case CORE_TOKEN_KVOID: /* void */
      return "CORE_TOKEN_KVOID";
    case CORE_TOKEN_KI8: /* i8_t */
      return "CORE_TOKEN_KI8";
    case CORE_TOKEN_KU8: /* u8_t */
      return "CORE_TOKEN_KU8";
    case CORE_TOKEN_KI16: /* i16_t */
      return "CORE_TOKEN_KI16";
    case CORE_TOKEN_KU16: /* u16_t */
      return "CORE_TOKEN_KU16";
    case CORE_TOKEN_KI32: /* i32_t */
      return "CORE_TOKEN_KI32";
    case CORE_TOKEN_KU32: /* u32_t */
      return "CORE_TOKEN_KU32";
    case CORE_TOKEN_KI64: /* i64_t */
      return "CORE_TOKEN_KI64";
    case CORE_TOKEN_KU64: /* u64_t */
      return "CORE_TOKEN_KU64";
    case CORE_TOKEN_KF32: /* f32_t */
      return "CORE_TOKEN_KF32";
    case CORE_TOKEN_KF64: /* f64_t */
      return "CORE_TOKEN_KF64";
    case CORE_TOKEN_KC32: /* c32_t */
      return "CORE_TOKEN_KC32";
    case CORE_TOKEN_KC64: /* c64_t */
      return "CORE_TOKEN_KC64";
    case CORE_TOKEN_KSTRING: /* string_t */
      return "CORE_TOKEN_KSTRING";
    case CORE_TOKEN_EOI:
      return "CORE_TOKEN_EOI";
    default:
      return "CORE_TOKEN_UNKNOWN";
    }
  }
  return "CORE_TOKEN_INVALID";
}

core_boolean_t core_token_print(core_lvm_p lvm, core_token_p token)
{
  if (!lvm) {
    fprintf(stderr, "core_token_print: no lvm\n");
    return core_boolean_false;
  }
  if (!token) {
    fprintf(lvm->whine, "core_token_print: no token\n");
    return core_boolean_false;
  } else {
    if (CORE_TOKEN_UNKNOWN != token->kind &&
        CORE_TOKEN_INVALID != token->kind) {
      fprintf(lvm->print, "token(%s, \"%s\", %lu, %lu, %lu)\n",
          core_token_name(lvm, token->kind), token->text->data,
          token->line, token->column, token->text->size);
    } else {
      fprintf(lvm->print, "token(%s, \"%s\", %lu, %lu, %lu)\n",
          core_token_name(lvm, token->kind), token->text->data,
          token->line, token->column, token->text->size);
      return core_boolean_false;
    }
  }
  return core_boolean_true;
}

core_boolean_t core_token_destroy(core_lvm_p lvm, core_token_p token)
{
  if (!lvm) {
    fprintf(stderr, "core_token_destroy: no lvm\n");
    return core_boolean_false;
  }
  if (core_lvm_free(lvm, token)) {
    fprintf(lvm->whine, "core_token_destroy: core_lvm_free failed\n");
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
  self->nl = core_boolean_false;
  self->ready = core_boolean_false;
  self->nested = 0;
  self->first = NULL;
  self->last = NULL;
  return core_boolean_true;
}

core_boolean_t core_reader_source(core_lvm_p lvm, core_reader_p self,
    core_text_p source)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_source: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(stderr, "core_reader_source: no self\n");
    return core_boolean_false;
  }
  if (!source) {
    fprintf(stderr, "core_reader_source: no source\n");
    return core_boolean_false;
  }
  self->source = source;
  self->start = source->data;
  self->current = source->data;
  self->line = 1;
  self->column = 1;
  lvm->line = 1;
  lvm->column = 1;
  self->ready = core_boolean_true;
  return core_boolean_true;
}

core_i8_t core_reader_peek(core_lvm_p lvm, core_reader_p self,
    core_boolean_t next)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_peek: no lvm\n");
    return '\0';
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_peek: no self\n");
    return '\0';
  } else {
    if (!self->current[0]) {
      return '\0';
    } else {
      return self->current[next];
    }
  }
}

core_i8_t core_reader_advance(core_lvm_p lvm, core_reader_p self,
    core_boolean_t next)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_advance: no lvm\n");
    return '\0';
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_advance: no self\n");
    return '\0';
  }
  if (!self->current[0]) {
    return '\0';
  } else {
    core_i8_t result = self->current[next];
    if (self->nl) {
      self->line++;
      self->column = 0;
      self->nl = core_boolean_false;
    }
    if ('\n' == self->current[0]) {
      self->nl = core_boolean_true;
    }
    self->column++;
    self->current++;
    return result;
  }
}

core_boolean_t core_reader_shift(core_lvm_p lvm, core_reader_p self)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_shift: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_shift: no self\n");
    return core_boolean_false;
  }
  self->start = self->current;
  lvm->line = self->line;
  lvm->column = self->column;
  return core_boolean_true;
}

core_i8_t core_reader_consume(core_lvm_p lvm, core_reader_p self, core_i8_t c)
{
  core_i8_t result = '\0';
  core_u64_t size = 0;
  core_token_kind kind = CORE_TOKEN_UNKNOWN;
  switch (c) {
  case ' ':
    kind = CORE_TOKEN_SPACE;
    break;
  case '\t':
    kind = CORE_TOKEN_TAB;
    break;
  case '\n':
    kind = CORE_TOKEN_NL;
    break;
  case '\r':
    kind = CORE_TOKEN_CR;
    break;
  case '\f':
    kind = CORE_TOKEN_FF;
    break;
  }
  do {
    lvm->line = self->line;
    lvm->column = self->column;
    size++;
    result = core_reader_advance(lvm, self, core_boolean_true);
    if (result && (c != result || '\n' == result || '\r' == result)) {
      core_token_p token = NULL;
      if (!(token = core_token_create(lvm, kind))) {
        fprintf(lvm->whine, "core_reader_consume:"
            " core_token_create failed\n");
        return '\0';
      }
      if (!(token->text = core_text_create(lvm, self->start, size))) {
        fprintf(lvm->whine, "core_reader_consume:"
            " core_text_create failed\n");
        return '\0';
      }
      size = 0;
    }
  } while (result && c == result && '\n' != result && '\r' != result);
  return result;
}

core_boolean_t core_reader_prefix(core_lvm_p lvm, core_reader_p self,
    core_i8_p prefix)
{
  if (!lvm) {
    fprintf(stderr, "core_reader_prefix: no lvm\n");
    return core_boolean_false;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_prefix: no self\n");
    return core_boolean_false;
  }
  if (!prefix) {
    fprintf(lvm->whine, "core_reader_prefix: no prefix\n");
    return core_boolean_false;
  }
  if (prefix[0]) {
    if (prefix[0] != core_reader_peek(lvm, self, core_boolean_false)) {
      return core_boolean_false;
    } else {
      if (prefix[1]) {
        if (prefix[1] != core_reader_peek(lvm, self, core_boolean_true)) {
          return core_boolean_false;
        } else {
          if (prefix[2]) {
            fprintf(lvm->whine, "core_reader_prefix: prefix too long\n");
            return core_boolean_false;
          } else {
            return core_boolean_true;
          }
        }
      } else {
        return core_boolean_true;
      }
    }
  } else {
    fprintf(lvm->whine, "core_reader_prefix: prefix is empty\n");
    return core_boolean_false;
  }
}

core_i8_t core_reader_whitespace(core_lvm_p lvm, core_reader_p self)
{
  core_i8_t result = '\0';
  core_token_p token = NULL;
  if (!lvm) {
    fprintf(stderr, "core_reader_whitespace: no lvm\n");
    return '\0';
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_whitespace: no self\n");
    return '\0';
  }
  result = core_reader_peek(lvm, self, core_boolean_false);
  while (isspace(result) || '/' == result) {
    if (self->nested || '/' == result) {
      if (core_reader_prefix(lvm, self, "//")) {
        if (!self->nested) {
          core_reader_shift(lvm, self);
        }
        core_reader_advance(lvm, self, core_boolean_false);
        result = core_reader_advance(lvm, self, core_boolean_true);
        while (result && '\n' != result) {
          result = core_reader_advance(lvm, self, core_boolean_true);
        }
        if (!self->nested) {
          if (!(token = core_token_create(lvm, CORE_TOKEN_COMMENT))) {
            fprintf(lvm->whine, "core_reader_whitespace:"
                " core_token_create failed\n");
            return '\0';
          }
        }
      } else if (core_reader_prefix(lvm, self, "/*")) {
        if (!self->nested) {
          core_reader_shift(lvm, self);
        }
        core_reader_advance(lvm, self, core_boolean_false);
        result = core_reader_advance(lvm, self, core_boolean_true);
        while (result && !core_reader_prefix(lvm, self, "*/")) {
          if (result && core_reader_prefix(lvm, self, "/*")) {
            self->nested += 1;
            result = core_reader_whitespace(lvm, self);
            self->nested -= 1;
          } else {
            result = core_reader_advance(lvm, self, core_boolean_true);
          }
        }
        core_reader_advance(lvm, self, core_boolean_false);
        result = core_reader_advance(lvm, self, core_boolean_true);
        if (!self->nested) {
          if (!(token = core_token_create(lvm, CORE_TOKEN_COMMENT))) {
            fprintf(lvm->whine, "core_reader_whitespace:"
                " core_token_create failed\n");
            return '\0';
          }
        }
      } else {
        return '/';
      }
      continue;
    } else {
      switch (result) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '\f':
        if (!self->nested) {
          core_reader_shift(lvm, self);
        }
        result = core_reader_consume(lvm, self, result);
        continue;
      default:
        return result;
      }
    }
  }
  return result;
}

core_token_p core_reader_number(core_lvm_p lvm, core_reader_p self)
{
  core_token_p result = NULL;
  core_i8_t c = '\0';
  core_u32_t base = 10;
  core_u32_t size = 0;
  if (!lvm) {
    fprintf(stderr, "core_reader_number: no lvm\n");
    return NULL;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_number: no self\n");
    return NULL;
  }
  c = core_reader_peek(lvm, self, core_boolean_false);
  switch (c) {
  case '0':
    c = core_reader_advance(lvm, self, core_boolean_true);
    switch (c) {
    case 'b':
    case 'B':
      base = 2;
      c = core_reader_advance(lvm, self, core_boolean_true);
      while (c & ('0' == c || '1' == c || '_' == c)) {
        c = core_reader_advance(lvm, self, core_boolean_false);
      }
      break;
    case 'o':
    case 'O':
      base = 8;
      c = core_reader_advance(lvm, self, core_boolean_true);
      while (c && (('0' <= c && '7' >= c) || '_' == c)) {
        c = core_reader_advance(lvm, self, core_boolean_false);
      }
      break;
    case 'd':
    case 'D':
      base = 10;
      c = core_reader_advance(lvm, self, core_boolean_true);
      while (c && (isdigit(c) || '_' == c)) {
        c = core_reader_advance(lvm, self, core_boolean_false);
      }
      break;
    case 'x':
    case 'X':
      base = 16;
      c = core_reader_advance(lvm, self, core_boolean_true);
      while (c && (isxdigit(c) || '_' == c)) {
        c = core_reader_advance(lvm, self, core_boolean_false);
      }
      break;
    }
    break;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    base = 10;
    c = core_reader_advance(lvm, self, core_boolean_true);
    while (c && (isdigit(c) || '_' == c)) {
      c = core_reader_advance(lvm, self, core_boolean_false);
    }
    break;
  default:
    fprintf(lvm->whine, "core_reader_number: not a number\n");
    return NULL;
  }
  if (c && ('.' == c || 'e' == c || 'E' == c)) {
    if ('.' == c) {
      c = core_reader_advance(lvm, self, core_boolean_true);
      switch (base) {
      case 2:
        while (c && (('0' == c || '1' == c) || '_' == c)) {
          c = core_reader_advance(lvm, self, core_boolean_false);
        }
        break;
      case 8:
        while (c && (('0' <= c && '7' >= c) || '_' == c)) {
          c = core_reader_advance(lvm, self, core_boolean_false);
        }
        break;
      case 10:
        while (c && (isdigit(c) || '_' == c)) {
          c = core_reader_advance(lvm, self, core_boolean_false);
        }
        break;
      case 16:
        while (c && (isxdigit(c) || '_' == c)) {
          c = core_reader_advance(lvm, self, core_boolean_false);
        }
        break;
      }
    }
    if (c && ('e' == c || 'E' == c)) {
      c = core_reader_whitespace(lvm, self);
      if (c && ('+' == c || '-' == c)) {
        c = core_reader_whitespace(lvm, self);
      }
      while (c && (isdigit(c) || '_' == c)) {
        c = core_reader_advance(lvm, self, core_boolean_true);
      }
    }
    if (!(result = core_token_create(lvm, CORE_TOKEN_DECIMAL))) {
      fprintf(lvm->whine, "core_reader_number: core_token_create failed\n");
      return NULL;
    }
  } else {
    if (!(result = core_token_create(lvm, CORE_TOKEN_INTEGER))) {
      fprintf(lvm->whine, "core_reader_number: core_token_create failed\n");
      return NULL;
    }
  }
  size = self->current - self->start;
  if (!(result->text = core_text_create(lvm, self->start, size))) {
    fprintf(lvm->whine, "core_reader_number: core_text_create failed\n");
    return NULL;
  }
  return result;
}

core_token_p core_reader_identifier(core_lvm_p lvm, core_reader_p self)
{
  core_token_p result = NULL;
  core_u64_t size = 0;
  core_i8_t c = '\0';
  if (!lvm) {
    fprintf(stderr, "core_reader_identifier: no lvm\n");
    return NULL;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_identifier: no self\n");
    return NULL;
  } else {
    c = core_reader_peek(lvm, self, core_boolean_false);
    while (c && (isalnum(c) || '_' == c)) {
      c = core_reader_advance(lvm, self, core_boolean_true);
    }
    if (!(result = core_token_create(lvm, CORE_TOKEN_IDENTIFIER))) {
      fprintf(lvm->whine, "core_reader_identifier: core_token_create failed\n");
      return NULL;
    }
    size = self->current - self->start;
    if (!(result->text = core_text_create(lvm, self->start, size))) {
      fprintf(lvm->whine, "core_reader_identifier: core_text_create failed\n");
      return NULL;
    }
  }
  return result;
}

core_token_p core_reader_character(core_lvm_p lvm, core_reader_p self)
{
  core_token_p result = NULL;
  core_i8_t c = '\0';
  core_u32_t size = 0;
  core_boolean_t escape = core_boolean_false;
  core_boolean_t valid = core_boolean_false;
  if (!lvm) {
    fprintf(stderr, "core_reader_character: no lvm\n");
    return NULL;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_character: no self\n");
    return NULL;
  }
  c = core_reader_advance(lvm, self, core_boolean_true);
  if ('\\' == c) {
    size++;
    escape = core_boolean_true;
  }
  if (escape) {
    c = core_reader_advance(lvm, self, core_boolean_true);
    switch (c) {
    case '\'':
    case '\\':
    case 'a':
    case 'b':
    case 'v':
    case '"':
    case '?':
    case 't':
    case 'n':
    case 'r':
    case 'f':
      c = core_reader_advance(lvm, self, core_boolean_true);
      size++;
      c = core_reader_advance(lvm, self, core_boolean_false);
      if ('\'' == c) {
        valid = core_boolean_true;
      }
      break;
    case 'x':
      c = core_reader_advance(lvm, self, core_boolean_true);
      size++;
      while (c && isxdigit(c)) {
        c = core_reader_advance(lvm, self, core_boolean_true);
      }
      c = core_reader_advance(lvm, self, core_boolean_false);
      if ('\'' == c) {
        valid = core_boolean_true;
      }
      core_reader_advance(lvm, self, core_boolean_true);
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      do {
        core_i8_t digit[3];
        core_u32_t at = 0;
        while (c && ('0' <= c && '7' >= c)) {
          c = core_reader_advance(lvm, self, core_boolean_true);
          digit[at++] = c;
          size++;
          if (3 == at) {
            c = core_reader_advance(lvm, self, core_boolean_true);
            if ('\'' == c) {
              valid = core_boolean_true;
            }
            if ('4' <= digit[0] && '7' >= digit[0]) {
              valid = core_boolean_false;
            }
            break;
          }
        }
        if (3 > at) {
          valid = core_boolean_true;
        }
      } while (core_boolean_false);
      break;
    default:
      fprintf(lvm->whine, "core_reader_character: not an escaped character\n");
      return NULL;
    }
  } else {
    c = core_reader_advance(lvm, self, core_boolean_true);
    if ('\'' == c) {
      valid = core_boolean_true;
    }
    core_reader_advance(lvm, self, core_boolean_true);
  }
  if (valid) {
    if (!(result = core_token_create(lvm, CORE_TOKEN_CHARACTER))) {
      fprintf(lvm->whine, "core_reader_character: core_token_create failed\n");
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, self->start, size))) {
      fprintf(lvm->whine, "core_reader_character: core_text_create failed\n");
      return NULL;
    }
  } else {
    if (!(result = core_token_create(lvm, CORE_TOKEN_INVALID))) {
      fprintf(lvm->whine, "core_reader_character: core_token_create failed\n");
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, "invalid character", 17))) {
      fprintf(lvm->whine, "core_reader_character: core_text_create failed\n");
      return NULL;
    }
  }
  return result;
}

core_token_p core_reader_string(core_lvm_p lvm, core_reader_p self)
{
  core_token_p result = NULL;
  core_i8_t c = '\0';
  core_u64_t size = 0;
  if (!lvm) {
    fprintf(stderr, "core_reader_string: no lvm\n");
    return NULL;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_string: no self\n");
    return NULL;
  }
  c = core_reader_advance(lvm, self, core_boolean_true);
  while (c && ('"' != c)) {
    c = core_reader_advance(lvm, self, core_boolean_true);
    if ('\\' == c) {
      c = core_reader_advance(lvm, self, core_boolean_true);
    }
  }
  if (!(result = core_token_create(lvm, CORE_TOKEN_STRING))) {
    fprintf(lvm->whine, "core_reader_string: core_token_create failed\n");
    return NULL;
  }
  if ('"' != c) {
    if (!(result = core_token_create(lvm, CORE_TOKEN_INVALID))) {
      fprintf(lvm->whine, "core_reader_string: core_token_create failed\n");
      return NULL;
    }
    if (!(result->text = core_text_create(lvm, "unterminated string", 19))) {
      fprintf(lvm->whine, "core_reader_string: core_text_create failed\n");
      return NULL;
    }
  }
  size = self->current - self->start;
  if (!(result = core_token_create(lvm, CORE_TOKEN_STRING))) {
    fprintf(lvm->whine, "core_reader_string: core_token_create failed\n");
    return NULL;
  }
  if (!(result->text = core_text_create(lvm, self->start, size))) {
    fprintf(lvm->whine, "core_reader_string: core_text_create failed\n");
    return NULL;
  }
  return result;
}

core_token_p core_reader_scan(core_lvm_p lvm, core_reader_p self)
{
  core_token_p result = NULL;
  core_u64_t size = 0;
  core_i8_t c = '\0';
  core_i8_t next = '\0';
  if (!lvm) {
    fprintf(stderr, "core_reader_scan: no lvm\n");
    return NULL;
  }
  if (!self) {
    fprintf(lvm->whine, "core_reader_scan: no self\n");
    return NULL;
  }
  while (1) {
    core_token_kind kind = CORE_TOKEN_UNKNOWN;
    core_reader_whitespace(lvm, self);
    core_reader_shift(lvm, self);
    c = core_reader_peek(lvm, self, core_boolean_false);
    if (!c) {
      if (!(result = core_token_create(lvm, CORE_TOKEN_EOI))) {
        fprintf(lvm->whine, "core_reader_scan: core_token_create failed\n");
        return NULL;
      }
      if (!(result->text = core_text_create(lvm, NULL, 0))) {
        fprintf(lvm->whine, "core_reader_scan: core_text_create failed\n");
        return NULL;
      }
      return result;
    } else if (isdigit(c)) {
      if (!(result = core_reader_number(lvm, self))) {
        fprintf(lvm->whine, "core_reader_scan: core_reader_number failed\n");
        return NULL;
      }
      return result;
    } else if (isalpha(c) || '_' == c) {
      if (!(result = core_reader_identifier(lvm, self))) {
        fprintf(lvm->whine, "core_reader_scan: core_reader_identifier failed\n");
        return NULL;
      }
      return result;
    }
    next = core_reader_advance(lvm, self, core_boolean_true);
    switch (c) {
    case '\'':
      if (!(result = core_reader_character(lvm, self))) {
        fprintf(lvm->whine, "core_reader_scan: core_reader_character failed\n");
        return NULL;
      }
      break;
    case '"':
      if (!(result = core_reader_string(lvm, self))) {
        fprintf(lvm->whine, "core_reader_scan: core_reader_string failed\n");
        return NULL;
      }
      break;
    case '+':
      switch (next) {
      case '+':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_PLUSA;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_INCREMENT;
        break;
      default:
        kind = CORE_TOKEN_PLUS;
        break;
      }
      break;
    case '-':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_MINUSA;
        break;
      case '-':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_DECREMENT;
        break;
      case '>':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_ARROW;
        break;
      default:
        kind = CORE_TOKEN_MINUS;
        break;
      }
      break;
    case '*':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_ASTERISKA;
        break;
      default:
        kind = CORE_TOKEN_ASTERISK;
        break;
      }
      break;
    case '/':
      switch (next) {
      case '/':
      case '*':
        core_reader_whitespace(lvm, self);
        continue;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_SLASHA;
        break;
      default:
        kind = CORE_TOKEN_SLASH;
        break;
      }
      break;
    case '%':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_PERCENTA;
        break;
      default:
        kind = CORE_TOKEN_PERCENT;
        break;
      }
      break;
    case '^':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_CARETA;
        break;
      default:
        kind = CORE_TOKEN_CARET;
        break;
      }
      break;
    case '&':
      switch (next) {
      case '&':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_LOGAND;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_AMPERSANDA;
        break;
      default:
        kind = CORE_TOKEN_AMPERSAND;
        break;
      }
      break;
    case '|':
      switch (next) {
      case '|':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_LOGOR;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_PIPEA;
        break;
      default:
        kind = CORE_TOKEN_PIPE;
        break;
      }
      break;
    case '~':
      kind = CORE_TOKEN_TILDE;
      break;
    case '!':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_NE;
        break;
      default:
        kind = CORE_TOKEN_EXCLAMATION;
        break;
      }
      break;
    case '?':
      kind = CORE_TOKEN_QUESTION;
      break;
    case ':':
      kind = CORE_TOKEN_COLON;
      break;
    case '=':
      switch (next) {
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_EQ;
        break;
      default:
        kind = CORE_TOKEN_ASSIGNMENT;
        break;
      }
      break;
    case '<':
      switch (next) {
      case '<':
        next = core_reader_advance(lvm, self, core_boolean_true);
        switch (next) {
        case '=':
          core_reader_advance(lvm, self, core_boolean_false);
          kind = CORE_TOKEN_LSHIFTA;
          break;
        default:
          kind = CORE_TOKEN_LSHIFT;
          break;
        }
        break;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_LE;
        break;
      default:
        kind = CORE_TOKEN_LT;
        break;
      }
      break;
    case '>':
      switch (next) {
      case '>':
        next = core_reader_advance(lvm, self, core_boolean_true);
        switch (next) {
        case '=':
          core_reader_advance(lvm, self, core_boolean_false);
          kind = CORE_TOKEN_RSHIFTA;
          break;
        default:
          kind = CORE_TOKEN_RSHIFT;
          break;
        }
        break;
      case '=':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_GE;
        break;
      default:
        kind = CORE_TOKEN_GT;
        break;
      }
      break;
    case ';':
      kind = CORE_TOKEN_SEMICOLON;
      break;
    case ',':
      kind = CORE_TOKEN_COMMA;
      break;
    case '.':
      kind = CORE_TOKEN_DOT;
      switch (next) {
      case '.':
        next = core_reader_advance(lvm, self, core_boolean_true);
        switch (next) {
        case '.':
          core_reader_advance(lvm, self, core_boolean_false);
          kind = CORE_TOKEN_ELLIPSIS;
          break;
        default:
          kind = CORE_TOKEN_RANGE;
          break;
        }
        break;
      default:
        kind = CORE_TOKEN_DOT;
        break;
      }
      break;
    case '(':
      kind = CORE_TOKEN_LPAREN;
      break;
    case ')':
      kind = CORE_TOKEN_RPAREN;
      break;
    case '[':
      kind = CORE_TOKEN_LBRACKET;
      break;
    case ']':
      kind = CORE_TOKEN_RBRACKET;
      break;
    case '{':
      kind = CORE_TOKEN_LBRACE;
      break;
    case '}':
      kind = CORE_TOKEN_RBRACE;
      break;
    case '#':
      switch (next) {
      case '#':
        core_reader_advance(lvm, self, core_boolean_false);
        kind = CORE_TOKEN_CONCAT;
        break;
      default:
        kind = CORE_TOKEN_HASH;
        break;
      }
    default:
      kind = CORE_TOKEN_UNKNOWN;
      break;
    }
    if (!(result = core_token_create(lvm, kind))) {
      fprintf(lvm->whine, "core_reader_scan: core_reader_scan failed\n");
      return NULL;
    }
    size = self->current - self->start;
    if (!(result->text = core_text_create(lvm, self->start, size))) {
      fprintf(lvm->whine, "core_reader_scan: core_text_create failed\n");
      return NULL;
    }
    break;
  }
  return result;
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
  if (!core_reader_init(lvm, core_lvm_reader(lvm))) {
    fprintf(stderr, "core_lvm_init: core_reader_init failed\n");
    return core_boolean_false;
  }
  return core_boolean_true;
}

core_reader_p core_lvm_reader(core_lvm_p lvm)
{
  if (!lvm) {
    fprintf(stderr, "core_lvm_reader: no lvm\n");
    return NULL;
  }
  return &lvm->reader;
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
  if (!core_reader_source(lvm, core_lvm_reader(lvm), source)) {
    fprintf(stderr, "core_lvm_string: core_reader_source failed\n");
    return core_boolean_false;
  }
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
    case CORE_GC_TOKEN:
      if (!core_token_destroy(lvm, (core_token_p)gc)) {
        fprintf(stderr, "core_lvm_cleanup: core_token_destroy failed\n");
        result = core_boolean_false;
      }
      break;
    }
    gc = next;
  }
  return result;
}
#endif
