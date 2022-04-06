/* MIT License

Copyright (c) 2022 Bezděk Miroslav

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define GC_ON 1
#define GC_MARK 0x01
#define GC_PROTECT 0x02
#define GC_MARK_GET(gc) (((gc)->mark & GC_MARK))
#define GC_PROTECT_GET(gc) (((gc)->mark & GC_PROTECT))
#define GC_PROTECT_SET(gc) (((gc)->mark) = ((gc)->mark) | GC_PROTECT)
#define GC_PROTECT_UNSET(gc) (((gc)->mark) ~GC_PROTECT)
#define GC_TOTAL 16

typedef enum {false, true} bool;

struct gc_s;
typedef struct gc_s gc_t, *gc_p, **gc_pp;
struct text_s;
typedef struct text_s text_t, *text_p, **text_pp;
struct error_s;
typedef struct error_s error_t, *error_p;
struct comment_s;
typedef struct comment_s comment_t, *comment_p;
struct list_s;
typedef struct list_s list_t, *list_p;
struct vector_s;
typedef struct vector_s vector_t, *vector_p;
struct token_s;
typedef struct token_s token_t, *token_p;
struct mal_s;
typedef struct mal_s mal_t, *mal_p, **mal_pp;
struct lvm_s;
typedef struct lvm_s lvm_t, *lvm_p, **lvm_pp;

typedef enum {
  GC_TEXT, GC_ERROR, GC_COMMENT, GC_TOKEN, GC_LIST, GC_VECTOR, GC_MAL
} gc_type;

typedef enum {
  TOKEN_CURRENT, TOKEN_NEXT
} token_position;

typedef enum {
  CONSTANT_NIL, CONSTANT_FALSE, CONSTANT_TRUE, CONSTANT_COUNT
} constant_type;

typedef enum {
  ERROR_NONE, ERROR_READER, ERROR_RUNTIME, ERROR_PRINTER
} error_type;

struct gc_s {
  gc_type type;
  int mark;
  gc_p next;
};

struct text_s {
  gc_t gc;
  char *data;
  size_t count;
  size_t capacity;
};

struct error_s {
  gc_t gc;
  text_pp data;
  error_type *type;
  size_t count;
  size_t capacity;
};

struct comment_s {
  gc_t gc;
  text_pp data;
  size_t count;
  size_t capacity;
};

struct list_s {
  gc_t gc;
  mal_pp data;
  size_t count;
  size_t capacity;
};

struct vector_s {
  gc_t gc;
  mal_pp data;
  size_t count;
  size_t capacity;
};

typedef enum {
  TOKEN_EOI, TOKEN_NIL, TOKEN_COMMENT, TOKEN_BOOLEAN, TOKEN_QUOTE, TOKEN_COLON,
  TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_AT, TOKEN_LBRACKET, TOKEN_SLASH,
  TOKEN_RBRACKET, TOKEN_CARET, TOKEN_BACKTICK, TOKEN_LBRACE, TOKEN_RBRACE,
  TOKEN_TILDE_AT, TOKEN_TILDE, TOKEN_SYMBOL, TOKEN_INTEGER, TOKEN_DECIMAL,
  TOKEN_KEYWORD, TOKEN_STRING
} token_type;

typedef union {
  text_p eoi;
  text_p nil;
  text_p error;
  text_p comment;
  text_p boolean;
  text_p special;
  text_p symbol;
  text_p keyword;
  text_p list;
  text_p vector;
  text_p string;
  text_p number;
} token_value;

struct token_s {
  gc_t gc;
  token_type type;
  token_value as;
  size_t length;
  size_t line;
  size_t column;
};

typedef enum {
  MAL_EOI, MAL_ERROR, MAL_BOOLEAN, MAL_SYMBOL, MAL_KEYWORD, MAL_STRING,
  MAL_NIL, MAL_LIST, MAL_VECTOR, MAL_INTEGER, MAL_DECIMAL
} mal_type;

typedef union {
  text_p eoi;
  text_p error;
  bool boolean;
  text_p symbol;
  text_p keyword;
  text_p string;
  text_p nil;
  list_p list;
  vector_p vector;
  long integer;
  double decimal;
} mal_value;

struct mal_s {
  gc_t gc;
  mal_type type;
  mal_value as;
  token_p token;
  text_p signature;
  size_t hash;
};

struct lvm_s {
  struct {
    gc_p first;
    size_t count;
    size_t total;
    int mark;
  } gc;
  struct {
    char *str;
    size_t pos;
    size_t line;
    size_t column;
    token_p token[2];
    bool valid[2];
  } reader;
  error_p error;
  comment_p comment;
  mal_p constant[CONSTANT_COUNT];
};

text_p text_make(lvm_p this, char *str);
text_p text_append(lvm_p this, text_p text, char item);
text_p text_concat(lvm_p this, text_p text, char *item);
text_p text_concat_text(lvm_p this, text_p text, text_p item);
text_p text_escape(lvm_p this, text_p text);
text_p text_unescape(lvm_p this, text_p text);
text_p text_make_integer(lvm_p this, long item);
text_p text_make_decimal(lvm_p this, double item);
long text_to_integer(lvm_p this, text_p p);
double text_to_decimal(lvm_p this, text_p p);
int text_cmp(lvm_p this, text_p text, char *item);
int text_cmp_text(lvm_p this, text_p text, text_p item);
size_t text_hash_fnv_1a(lvm_p this, text_p p);
size_t text_hash_jenkins(lvm_p this, text_p p);
text_p text_display_position(lvm_p this, token_p token, char *text);
char *text_str(lvm_p this, text_p text);
void text_free(lvm_p this, gc_p text);
bool error_make(lvm_p this);
text_p error_append(lvm_p this, error_type type, text_p text);
text_p error_collapse(lvm_p this);
void error_free(lvm_p this, gc_p gc);
bool comment_make(lvm_p this);
text_p comment_append(lvm_p this, text_p text);
text_p comment_collapse(lvm_p this);
void comment_free(lvm_p this, gc_p gc);
list_p list_make(lvm_p this, size_t init);
bool list_append(lvm_p this, list_p list, mal_p mal);
text_p list_text(lvm_p this, list_p list);
void list_free(lvm_p this, gc_p list);
vector_p vector_make(lvm_p this, size_t init);
bool vector_append(lvm_p this, vector_p vector, mal_p mal);
text_p vector_text(lvm_p this, vector_p vector);
void vector_free(lvm_p this, gc_p vector);
#if __STDC__
/* C89 */
#ifndef __STDC_VERSION__
char *strdup(char *str);
char *strndup(char *str, size_t n);
#else
; /* C90[+] */
#endif
#endif
char *readline(lvm_p this, char *prompt);
char tokenizer_peek(lvm_p this);
char tokenizer_peek_next(lvm_p this);
char tokenizer_next(lvm_p this);
token_p tokenizer_scan(lvm_p this);
token_p token_make(lvm_p this);
token_p token_eoi(lvm_p this);
token_p token_comment(lvm_p this);
token_p token_special(lvm_p this);
token_p token_number(lvm_p this);
token_p token_symbol(lvm_p this);
token_p token_keyword(lvm_p this);
token_p token_string(lvm_p this);
void token_free(lvm_p this, gc_p gc);
token_p reader_peek(lvm_p this);
token_p reader_peek_next(lvm_p this);
token_p reader_next(lvm_p this);
mal_p lvm_read_str(lvm_p this);
mal_p lvm_read_form(lvm_p this);
mal_p lvm_read_list(lvm_p this);
mal_p lvm_read_parenthesis(lvm_p this);
mal_p lvm_read_vector(lvm_p this);
mal_p lvm_read_brackets(lvm_p this);
mal_p lvm_read_atom(lvm_p this);
mal_p lvm_mal_make(lvm_p this, mal_type type);
mal_p lvm_mal_eoi(lvm_p this);
mal_p lvm_mal_nil(lvm_p this);
mal_p lvm_mal_error(lvm_p this, error_type type, text_p text);
mal_p lvm_mal_boolean(lvm_p this, bool boolean);
mal_p lvm_mal_symbol(lvm_p this, text_p symbol);
mal_p lvm_mal_keyword(lvm_p this, text_p keyword);
mal_p lvm_mal_string(lvm_p this, text_p string);
mal_p lvm_mal_list(lvm_p this, list_p list);
mal_p lvm_mal_vector(lvm_p this, vector_p vector);
mal_p lvm_mal_integer(lvm_p this, long integer);
mal_p lvm_mal_decimal(lvm_p this, double decimal);
text_p lvm_mal_print(lvm_p this, mal_p mal, bool readable);
void lvm_mal_free(lvm_p this, gc_p mal);
lvm_p lvm_make();
void lvm_gc_print(lvm_p this);
void lvm_gc(lvm_p this);
void lvm_gc_free(lvm_p this);
void lvm_free(lvm_pp this);
mal_p lvm_read(lvm_p this, char *str);
mal_p lvm_eval(lvm_p this, mal_p ast);
char *lvm_print(lvm_p this, mal_p value);
char *lvm_rep(lvm_p this, char *str);

text_p text_make(lvm_p this, char *str)
{
  size_t size = strlen(str);
  text_p text = (text_p)calloc(1, sizeof(text_t));
  text->count = size;
  text->capacity = ((0 != (size % 32)) + (size / 32)) * 32;
  text->data = (char *)calloc((text->capacity + 1), sizeof(char));
  strncpy(text->data, str, size);
  text->data[size] = 0x00;
  text->gc.type = GC_TEXT;
#if GC_ON
  text->gc.mark = !this->gc.mark;
#else
  text->gc.mark = this->gc.mark;
#endif
  text->gc.next = this->gc.first;
  this->gc.first = (gc_p)text;
  this->gc.count++;
  return text;
}

text_p text_append(lvm_p this, text_p text, char item)
{
  (void)this;
  if (text->count + 1 >= text->capacity) {
    text->capacity = text->capacity + 32;
    text->data = (char *)realloc(text->data, (text->capacity + 1) * sizeof (char));
  }
  text->data[text->count] = item;
  if (item) {
    text->data[++text->count] = 0x00;
  }
  return text;
}

text_p text_concat(lvm_p this, text_p text, char *item)
{
  size_t size = strlen(item);
  (void)this;
  if (text->count + size + 1 >= text->capacity) {
    text->capacity = ((0 != ((text->count + size) % 32))
        + ((text->count + size) / 32)) * 32;
    text->data = (char *)realloc(text->data,
        (text->capacity + 1) * sizeof(char));
  }
  if (size > 0) {
    strncpy(text->data + text->count, item, size);
    text->count = text->count + size;
    text->data[text->count] = 0x00;
  }
  return text;
}

text_p text_concat_text(lvm_p this, text_p text, text_p item)
{
  size_t size = item->count;
  (void)this;
  if (text->count + size + 1 >= text->capacity) {
    text->capacity = ((0 != ((text->count + size) % 32))
        + ((text->count + size) / 32)) * 32;
    text->data = (char *)realloc(text->data,
        (text->capacity + 1) * sizeof(char));
  }
  if (size > 0) {
    strncpy(text->data + text->count, item->data, size);
    text->count = text->count + size;
    text->data[text->count] = 0x00;
  }
  return text;
}

text_p text_escape(lvm_p this, text_p text)
{
  text_p escaped = text_make(this, "");
  char *string = text->data;
  text_append(this, escaped, '"');
  while (0x00 != *string) {
    switch (*string) {
    case 0x09:
      text_append(this, escaped, 0x5C);
      text_append(this, escaped, 't');
      break;
    case 0x0A:
      text_append(this, escaped, 0x5C);
      text_append(this, escaped, 'n');
      break;
    case 0x0D:
      text_append(this, escaped, 0x5C);
      text_append(this, escaped, 'r');
      break;
    case '"':
      text_append(this, escaped, 0x5C);
      text_append(this, escaped, '"');
      break;
    case 0x5C:
      text_append(this, escaped, 0x5C);
      text_append(this, escaped, 0x5C);
      break;
    default:
      text_append(this, escaped, *string);
      break;
    }
    string++;
  }
  text_append(this, escaped, '"');
  return escaped;
}

text_p text_unescape(lvm_p this, text_p text)
{
  text_p unescaped = text_make(this, "");
  size_t index = 1;
  text_append(this, unescaped, '"');
  for (; index < text->count; index++) {
    if (0x5C == text->data[index]) {
      switch (text->data[index + 1]) {
      case '"':
        text_append(this, unescaped, '"');
        index++;
        break;
      case 0x5C:
        text_append(this, unescaped, 0x5C);
        index++;
        break;
      case 'n':
        text_append(this, unescaped, 0x0A);
        index++;
        break;
      case 'r':
        text_append(this, unescaped, 0x0D);
        index++;
        break;
      case 't':
        text_append(this, unescaped, 0x09);
        index++;
        break;
      default:
        text_append(this, unescaped, text->data[index]);
        break;
      }
    } else if (index != text->count && '"' != text->data[index]) {
      text_append(this, unescaped, text->data[index]);
    }
  }
  return unescaped;
}

text_p text_make_integer(lvm_p this, long item)
{
  text_p result = text_make(this, "");
  (void)this;
  if (!item) {
    text_append(this, result, '0');
  } else {
    size_t from = result->count;
    size_t to, tmp;
    result->count = 0;
    while (item) {
      text_append(this, result, (item % 10) + '0');
      item /= 10;
    }
    to = result->count - 1;
    while (from < to) {
      tmp = result->data[from];
      result->data[from++] = result->data[to];
      result->data[to--] = tmp;
    }
  }
  result->data[result->count] = 0x00;
  return result;
}

text_p text_make_decimal(lvm_p this, double item)
{
  text_p result = text_make(this, "");
  long integer = (long)item;
  double fractional = item - integer;
  int digits = 0;
  (void)this;

  if (0 == integer) {
    text_append(this, result, '0');
  } else {
    int from = result->count;
    int to, tmp;
    while (integer && digits < 16) {
      text_append(this, result, (integer % 10) + '0');
      integer /= 10;
      digits++;
    }
    while (integer && digits >= 16) {
      text_append(this, result, '0');
      integer /= 10;
    }

    to = result->count - 1;
    while (from < to) {
      tmp = result->data[from];
      result->data[from++] = result->data[to];
      result->data[to--] = tmp;
    }
    result->data[result->count] = 0x00;
  }
  if (!fractional) {
    text_append(this, result, '.');
    text_append(this, result, '0');
  } else {
    text_append(this, result, '.');
    integer = fractional *= 10;
    while (fractional > 0 && digits < 16) {
      text_append(this, result, (integer % 10) + '0');
      integer = fractional *= 10;
      fractional -= integer;
      digits++;
    }
    while (fractional > 0 && digits >= 16) {
      integer = fractional *= 10;
      fractional -= integer;
    }
  }
  return result;
}

long text_to_integer(lvm_p this, text_p text)
{
  long value = 0x00;
  size_t at = 0x00;
  (void)this;
  for (at = 0x00; at < text->count; at++) {
    char ch = text->data[at];
    if ('0' <= ch && '9' >= ch) {
      value = value * 0x0A + (long)(ch - '0');
    } else {
      return value;
    }
  }
  return value;
}

double text_to_decimal(lvm_p this, text_p text)
{
  double value = 0x00;
  double multiplier;
  int decimal = 0;
  size_t at = 0x00;
  (void)this;
  for (at = 0x00; at < text->count; at++) {
    char ch = text->data[at];
    if ('0' <= ch && '9' >= ch && !decimal) {
      value = value * 0x0A + (long)(ch - '0');
    } else if ('0' <= ch && '9' >= ch && decimal) {
      multiplier = pow(0.1, decimal);
      value += (multiplier) * (double)(ch - '0');
      decimal++;
    } else if ('.' == ch || 'E' == ch || 'e' == ch) {
      decimal = 1;
    }
  }
  return value;
}

int text_cmp(lvm_p this, text_p text, char *item)
{
  size_t size = strlen(item);
  (void)this;
  return strncmp(text->data, item, size);
}

int text_cmp_text(lvm_p this, text_p text, text_p item)
{
  (void)this;
  return strncmp(text->data, item->data, text->count);
}

size_t text_hash_fnv_1a(lvm_p this, text_p text)
{
  size_t hash = 2166136261U;
  unsigned int i = 0;
  (void)this;
  for (; i < text->count; i++) {
    hash ^= (unsigned char)text->data[i];
    hash *= 16777619;
  }
  return hash;
}

size_t text_hash_jenkins(lvm_p this, text_p text)
{
  size_t hash = 0;
  unsigned char *str = (unsigned char *)text->data;
  (void)this;
  for (; *str; str++) {
    hash += *str;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

text_p text_display_position(lvm_p this, token_p token, char *text)
{
  return text_append(this, text_concat_text(this, text_concat_text(this,
      text_make(this, "L"), text_make_integer(this, token->line)),
      text_concat_text(this, text_append(this, text_concat_text(this,
      text_make(this, " C"), text_make_integer(this, token->column)), ' '),
      text_make(this, text))), '\n');
}

char *text_str(lvm_p this, text_p text)
{
  (void)this;
  return strdup(text->data);
}

void text_free(lvm_p this, gc_p text)
{
  (void)this;
  free((void *)((text_p)text)->data);
  free((void *)((text_p)text));
}

bool error_make(lvm_p this)
{
  this->error = (error_p)calloc(1, sizeof(error_t));
  this->error->count = 0;
  this->error->capacity = 32;
  this->error->data = (text_pp)calloc((this->error->capacity), sizeof (text_p));
  this->error->type = (error_type *)calloc((this->error->capacity),
      sizeof (error_type));
  this->error->gc.type = GC_ERROR;
#if GC_ON
  this->error->gc.mark = !this->gc.mark;
#else
  this->error->gc.mark = this->gc.mark;
#endif
  this->error->gc.next = this->gc.first;
  this->gc.first = (gc_p)this->error;
  this->gc.count++;
  return true;
}

text_p error_append(lvm_p this, error_type type, text_p text)
{
  if (!this->error) {
    this->error = (error_p)calloc(1, sizeof(error_t));
    this->error->count = 0;
    this->error->capacity = 0;
    this->error->gc.type = GC_ERROR;
#if GC_ON
    this->error->gc.mark = !this->gc.mark;
#else
    this->error->gc.mark = this->gc.mark;
#endif
    this->error->gc.next = this->gc.first;
    this->gc.first = (gc_p)this->error;
    this->gc.count++;
  }
  if (this->error->count + 1 >= this->error->capacity) {
    this->error->capacity += 32;
    this->error->data = (text_pp)realloc(this->error->data,
        (this->error->capacity) * sizeof (text_p));
    this->error->type = (error_type *)realloc(this->error->type,
        (this->error->capacity) * sizeof (error_type));
  }
  this->error->type[this->error->count] = type;
  this->error->data[this->error->count++] = text;
  return text;
}

text_p error_collapse(lvm_p this)
{
  text_p text;
  size_t i;
  text = text_make(this, "([ERROR]\n");
  if (this->error) {
    for (i = 0; i < this->error->count; i++) {
      switch (this->error->type[i]) {
      case ERROR_NONE:
        text_concat(this, text, "  [OK] ");
        break;
      case ERROR_READER:
        text_concat(this, text, "  [READER] ");
        break;
      case ERROR_RUNTIME:
        text_concat(this, text, "  [RUNTIME] ");
        break;
      case ERROR_PRINTER:
        text_concat(this, text, "  [PRINTER] ");
        break;
      default:
        text_concat(this, text, "  [UNKNOWN] ");
      }
      text_concat_text(this, text, this->error->data[i]);
    }
  }
  text_concat(this, text, ")\n");
  return text;
}

bool error_empty(lvm_p this)
{
  if (!this->error) {
    return true;
  }
  return 0 == this->error->count;
}

void error_free(lvm_p this, gc_p gc)
{
  (void)this;
  free((void *)((error_p)gc)->data);
  free((void *)((error_p)gc)->type);
  free((void *)((error_p)gc));
}

bool comment_make(lvm_p this)
{
  this->comment = (comment_p)calloc(1, sizeof(comment_t));
  this->comment->count = 0;
  this->comment->capacity = 32;
  this->comment->data = (text_pp)realloc(this->comment->data,
      (this->comment->capacity) * sizeof (text_p));
  this->comment->gc.type = GC_COMMENT;
#if GC_ON
  this->comment->gc.mark = !this->gc.mark;
#else
  this->comment->gc.mark = this->gc.mark;
#endif
  this->comment->gc.next = this->gc.first;
  this->gc.first = (gc_p)this->comment;
  this->gc.count++;
  return true;
}

text_p comment_append(lvm_p this, text_p text)
{
  if (!this->comment) {
    this->comment = (comment_p)calloc(1, sizeof(comment_t));
    this->comment->count = 0;
    this->comment->capacity = 0;
  }
  if (this->comment->count + 1 >= this->comment->capacity) {
    this->comment->capacity += 32;
    this->comment->data = (text_pp)realloc(this->comment->data,
        (this->comment->capacity) * sizeof (text_p));
  }
  this->comment->data[this->comment->count++] = text;
  this->comment->data[this->comment->count] = NULL;
  return text;
}

text_p comment_collapse(lvm_p this)
{
  text_p text;
  size_t i;
  text = text_make(this, "([comment]\n");
  if (this->comment) {
    for (i = 0; i < this->comment->count; i++) {
      text_concat_text(this, text, this->comment->data[i]);
    }
  }
  text_concat(this, text, ")\n");
  return text;
}

void comment_free(lvm_p this, gc_p gc)
{
  (void)this;
  free((void *)((comment_p)gc)->data);
  free((void *)gc);
}

list_p list_make(lvm_p this, size_t init)
{
  list_p list = (list_p)calloc(1, sizeof(list_t));
  size_t capacity = 2;
  (void)this;
  if (0 == init) {
    init = 2;
  }
  while (capacity < init) {
    capacity = (capacity << 1);
  }
  list->count = 0;
  list->capacity = capacity;
  list->data = (mal_pp)calloc(capacity, sizeof(mal_p));
  list->gc.type = GC_LIST;
#if GC_ON
  list->gc.mark = !this->gc.mark;
#else
  list->gc.mark = this->gc.mark;
#endif
  list->gc.next = this->gc.first;
  this->gc.first = (gc_p)list;
  this->gc.count++;
  return list;
}

bool list_append(lvm_p this, list_p list, mal_p mal)
{
  (void)this;
  if (list->count >= list->capacity) {
    mal_pp tmp;
    list->capacity <<= 1;
    tmp = (mal_pp)realloc(list->data, list->capacity * sizeof(mal_p));
    if (NULL == tmp) {
      error_append(this, ERROR_RUNTIME, text_display_position(this,
          reader_peek(this), "not enough memory"));
      return false;
    }
    list->data = tmp;
  }
  list->data[list->count++] = mal;
  return true;
}

text_p list_text(lvm_p this, list_p list)
{
  text_p mal = text_make(this, "(");
  size_t i = 0;
  if (list->count > 0) {
    text_concat_text(this, mal, lvm_mal_print(this, list->data[0], false));
    for (i = 1; i < list->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, lvm_mal_print(this, list->data[i], false));
    }
    if (text_cmp(this, lvm_mal_print(this, list->data[i], false), "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, lvm_mal_print(this, list->data[i], false));
    }
  }
  return text_append(this, mal, ')');
}

void list_free(lvm_p this, gc_p list)
{
  (void)this;
  free((void *)((list_p)list)->data);
  free((void *)list);
}

vector_p vector_make(lvm_p this, size_t init)
{
  vector_p vector = (vector_p)calloc(1, sizeof(vector_t));
  size_t capacity = 2;
  (void)this;
  if (0 == init) {
    init = 2;
  }
  while (capacity < init) {
    capacity = (capacity << 1);
  }
  vector->count = 0;
  vector->capacity = capacity;
  vector->data = (mal_pp)calloc(capacity, sizeof(mal_p));
  vector->gc.type = GC_VECTOR;
#if GC_ON
  vector->gc.mark = !this->gc.mark;
#else
  vector->gc.mark = this->gc.mark;
#endif
  vector->gc.next = this->gc.first;
  this->gc.first = (gc_p)vector;
  this->gc.count++;
  return vector;
}

bool vector_append(lvm_p this, vector_p vector, mal_p mal)
{
  (void)this;
  if (vector->count >= vector->capacity) {
    mal_pp tmp;
    vector->capacity <<= 1;
    tmp = (mal_pp)realloc(vector->data, vector->capacity * sizeof(mal_p));
    if (NULL == tmp) {
      error_append(this, ERROR_RUNTIME, text_display_position(this,
          reader_peek(this), "not enough memory"));
      return false;
    }
    vector->data = tmp;
  }
  vector->data[vector->count++] = mal;
  return true;
}

text_p vector_text(lvm_p this, vector_p vector)
{
  text_p mal = text_make(this, "[");
  size_t i;
  if (vector->count > 0) {
    text_concat_text(this, mal, lvm_mal_print(this, vector->data[0], false));
    for (i = 1; i < vector->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, lvm_mal_print(this, vector->data[i], false));
    }
    if (text_cmp(this, lvm_mal_print(this, vector->data[i], false), "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, lvm_mal_print(this, vector->data[i], false));
    }
  }
  return text_append(this, mal, ']');
}

void vector_free(lvm_p this, gc_p vector)
{
  (void)this;
  free((void *)((vector_p)vector)->data);
  free((void *)vector);
}

#if __STDC__
#ifndef __STDC_VERSION__
/* C89 */
char *strdup(char *str)
{
  char *result;
  char *p = str;
  size_t n = 0;

  while (*p++)
    n++;
  result = malloc(n * sizeof(char) + 1);
  p = result;
  while (*str)
    *p++ = *str++;
  *p = 0x00;
  return result;
}

char *strndup(char *str, size_t n)
{
  char *result;
  char *p;
  result = malloc(n * sizeof(char) + 1);
  p = result;
  while (*str)
    *p++ = *str++;
  *p = 0x00;
  return result;
}
#else
; /* C90[+] */
#endif
#endif

char *readline(lvm_p this, char *prompt)
{
  char buffer[2048], *tmp;
  (void)this;
  printf("%s", prompt);
  fgets(buffer, sizeof(buffer), stdin);
  if (feof(stdin)) {
    return NULL;
  }
  buffer[strcspn(buffer, "\r\n")] = 0x00;
  tmp = (char *)strndup(buffer, strlen(buffer));
  return tmp;
}

char tokenizer_peek(lvm_p this)
{ 
  return this->reader.str[this->reader.pos];
}

char tokenizer_peek_next(lvm_p this)
{
  if (0x00 == this->reader.str[this->reader.pos]) {
    return 0x00;
  } else {
    return this->reader.str[this->reader.pos + 1];
  }
}

char tokenizer_next(lvm_p this)
{
  if (0x00 == this->reader.str[this->reader.pos]) {
    return 0x00;
  } else {
    char ch = this->reader.str[this->reader.pos++];
    if (0x0A == ch) {
      this->reader.line++;
      this->reader.column = 0x00;
    } else {
      this->reader.column++;
    }
    return ch;
  }
}

token_p tokenizer_scan(lvm_p this)
{
  char ch;
  while (true) {
    while (isspace(ch = tokenizer_peek(this))) {
      switch (ch) {
      case 0x09:
      case 0x0A:
      case 0x20:
        tokenizer_next(this);
        break;
      default:
        tokenizer_next(this);
        break;
      }
    }
    switch (ch) {
    case 0x00:
      return token_eoi(this);
    case '"':
      return token_string(this);
    case 0x27:
    case '(':
    case ')':
    case '@':
    case '[':
    case 0x5C:
    case ']':
    case '^':
    case '`':
    case '{':
    case '}':
    case '~':
      return token_special(this);
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return token_number(this);
    case ':':
      if (isspace(tokenizer_peek_next(this))) {
        return token_special(this);
      } else {
        return token_keyword(this);
      }
    case ';':
      token_comment(this);
      continue;
    default:
      return token_symbol(this);
    }
  }
}

token_p token_make(lvm_p this)
{
  token_p token = (token_p)calloc(1, sizeof(token_t));
  token->line = 1;
  token->gc.type = GC_TOKEN;
#if GC_ON
  token->gc.mark = !this->gc.mark;
#else
  token->gc.mark = this->gc.mark;
#endif
  token->gc.next = this->gc.first;
  this->gc.first = (gc_p)token;
  this->gc.count++;
  return token;
}

token_p token_eoi(lvm_p this)
{
  token_p token = token_make(this);
  token->type = TOKEN_EOI;
  token->as.eoi = text_make(this, "");
  return token;
}

token_p token_comment(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  char ch;
  token->line = this->reader.line;
  token->column = this->reader.column;
  token->type = TOKEN_COMMENT;
  tokenizer_next(this);
  text_append(this, text, ' ');
  while (isspace(tokenizer_peek(this)) &&
      ((0x0A != tokenizer_peek(this)) ||
      (0x0D != tokenizer_peek(this)))) {
    tokenizer_next(this);
  }
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case 0x0A:
      text_append(this, text, 0x00);
      token->as.comment = text_display_position(this, token, text->data);
      comment_append(this, token->as.comment);
      return token;
    case 0x0D:
      text_append(this, text, 0x00);
      token->as.comment = text_display_position(this, token, text->data);
      return token;
    default:
      text_append(this, text, tokenizer_peek(this));
      break;
    }
    tokenizer_next(this);
  }
  token->as.comment = text_display_position(this, token, text->data);
  comment_append(this, token->as.comment);
  return token;
}

token_p token_special(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  unsigned int i;
  token->line = this->reader.line;
  token->column = this->reader.column;
  switch (tokenizer_peek(this)) {
  case 0x27:
    token->type = TOKEN_QUOTE;
    token->length = 1;
    break;
  case '(':
    token->type = TOKEN_LPAREN;
    token->length = 1;
    break;
  case ')':
    token->type = TOKEN_RPAREN;
    token->length = 1;
    break;
  case ':':
    token->type = TOKEN_COLON;
    token->length = 1;
    break;
  case '@':
    token->type = TOKEN_AT;
    token->length = 1;
    break;
  case '[':
    token->type = TOKEN_LBRACKET;
    token->length = 1;
    break;
  case 0x5C:
    token->type = TOKEN_SLASH;
    token->length = 1;
    break;
  case ']':
    token->type = TOKEN_RBRACKET;
    token->length = 1;
    break;
  case '^':
    token->type = TOKEN_CARET;
    token->length = 1;
    break;
  case '`':
    token->type = TOKEN_BACKTICK;
    token->length = 1;
    break;
  case '{':
    token->type = TOKEN_LBRACE;
    token->length = 1;
    break;
  case '}':
    token->type = TOKEN_RBRACE;
    token->length = 1;
    break;
  case '~':
    if ('@' == tokenizer_peek_next(this)) {
      token->type = TOKEN_TILDE_AT;
      token->length = 2;
      break;
    } else {
      token->type = TOKEN_TILDE;
      token->length = 1;
      break;
    }
  }
  for (i = 0; i < token->length; i++) {
    text_append(this, text, tokenizer_next(this));
  }
  text_append(this, text, 0x00);
  token->as.special = text;
  return token;
}

token_p token_number(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  char ch;
  bool decimal = false;
  token->line = this->reader.line;
  token->column = this->reader.column;
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case 'E':
    case 'e':
      if (('.' == ch || 'E' == ch || 'e' == ch) && !decimal) {
        decimal = true;
        text_append(this, text, ch);
        tokenizer_next(this);
        break;
      } else if ('.' != ch && 'E' != ch && 'e' != ch) {
        text_append(this, text, ch);
        tokenizer_next(this);
        break;
      } else {
        text_append(this, text, 0x00);
        token->length = text->count;
        token->as.number = text;
        if (decimal) {
          token->type = TOKEN_DECIMAL;
        } else {
          token->type = TOKEN_INTEGER;
        }
        return token;
      }
    case 0x09:
    case 0x0A:
    case 0x0D:
    case ' ':
    case '"':
    case 0x27:
    case '(':
    case ')':
    case ':':
    case '@':
    case '[':
    case 0x5C:
    case ']':
    case '^':
    case '`':
    case '{':
    case '}':
    case '~':
    default:
      text_append(this, text, 0x00);
      token->length = text->count;
      token->as.number = text;
      if (decimal) {
        token->type = TOKEN_DECIMAL;
      } else {
        token->type = TOKEN_INTEGER;
      }
      return token;
    }
  }
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.number = text;
  if (decimal) {
    token->type = TOKEN_DECIMAL;
  } else {
    token->type = TOKEN_INTEGER;
  }
  return token;
}

token_p token_symbol(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  char ch;
  token->type = TOKEN_SYMBOL;
  token->line = this->reader.line;
  token->column = this->reader.column;
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case 0x09:
    case 0x0A:
    case 0x0D:
    case ' ':
    case '"':
    case 0x27:
    case '(':
    case ')':
    case ':':
    case '@':
    case '[':
    case 0x5C:
    case ']':
    case '^':
    case '`':
    case '{':
    case '}':
    case '~':
      text_append(this, text, 0x00);
      token->length = text->count;
      token->as.symbol = text;
      return token;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.symbol = text;
  return token;
}

token_p token_keyword(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  char ch;
  token->type = TOKEN_KEYWORD;
  token->line = this->reader.line;
  token->column = this->reader.column;
  tokenizer_next(this);
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case 0x09:
    case 0x0A:
    case 0x0D:
    case ' ':
    case '"':
    case 0x27:
    case '(':
    case ')':
    case ':':
    case '@':
    case '[':
    case 0x5C:
    case ']':
    case '^':
    case '`':
    case '{':
    case '}':
    case '~':
      text_append(this, text, 0x00);
      token->length = text->count;
      token->as.keyword = text;
      return token;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.keyword = text;
  return token;
}

token_p token_string(lvm_p this)
{
  token_p token = token_make(this);
  text_p text = text_make(this, "");
  char ch;
  token->type = TOKEN_STRING;
  token->line = this->reader.line;
  token->column = this->reader.column;
  tokenizer_next(this);
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case '"':
      tokenizer_next(this);
      text_append(this, text, 0x00);
      token->length = text->count;
      token->as.string = text;
      return token;
    case 0x5C:
      tokenizer_next(this);
      switch (ch = tokenizer_next(this)) {
      case 't':
        text_append(this, text, 0x09);
        break;
      case 'n':
        text_append(this, text, 0x0A);
        break;
      case 'r':
        text_append(this, text, 0x0D);
        break;
      case '"':
        text_append(this, text, ch);
        break;
      case 0x5C:
        text_append(this, text, ch);
        break;
      default:
        ;
        break;
      }
      break;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  tokenizer_next(this);
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.string = text;
  return token;
}

void token_free(lvm_p this, gc_p gc)
{
  (void)this;
  free((void *)((token_p)gc));
}

token_p reader_peek(lvm_p this)
{
  if (this->reader.valid[TOKEN_CURRENT]) {
    return this->reader.token[TOKEN_CURRENT];
  } else {
    this->reader.valid[TOKEN_CURRENT] = true;
    return this->reader.token[TOKEN_CURRENT] = tokenizer_scan(this);
  }
}

token_p reader_peek_next(lvm_p this)
{
  if (this->reader.valid[TOKEN_NEXT]) {
    return this->reader.token[TOKEN_NEXT];
  } else {
    this->reader.valid[TOKEN_NEXT] = true;
    return this->reader.token[TOKEN_NEXT] = tokenizer_scan(this);
  }
}

token_p reader_next(lvm_p this)
{
  if (this->reader.valid[TOKEN_NEXT]) {
    this->reader.valid[TOKEN_NEXT] = false;
    return this->reader.token[TOKEN_CURRENT] = this->reader.token[TOKEN_NEXT];
  } else {
    this->reader.valid[TOKEN_CURRENT] = true;
    this->reader.valid[TOKEN_NEXT] = false;
    this->reader.token[TOKEN_CURRENT] = tokenizer_scan(this);
    return this->reader.token[TOKEN_CURRENT];
  }
}

mal_p lvm_read_str(lvm_p this)
{
  if (TOKEN_EOI == reader_peek(this)->type) {
    return lvm_mal_eoi(this);
  } else if (0 < this->error->count) {
    return lvm_mal_eoi(this);
  } else {
    return lvm_read_form(this);
  }
}

mal_p lvm_read_form(lvm_p this)
{
  token_p token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return lvm_mal_eoi(this);
  case TOKEN_LPAREN:
    return lvm_read_list(this);
  case TOKEN_RPAREN:
    return lvm_mal_error(this, ERROR_READER, text_display_position(this,
        token, "unbalanced parenthesis, expected '('"));
  case TOKEN_LBRACKET:
    return lvm_read_vector(this);
  case TOKEN_RBRACKET:
    return lvm_mal_error(this, ERROR_READER, text_display_position(this,
        token, "unbalanced brackets, expected '['"));
  default:
    return lvm_read_atom(this);
  }
}

mal_p lvm_read_list(lvm_p this)
{
  return lvm_read_parenthesis(this);
}

mal_p lvm_read_parenthesis(lvm_p this)
{
  token_p token = reader_next(this);
  list_p list = list_make(this, 0);
  mal_p mal;
  token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return lvm_mal_error(this, ERROR_READER, text_display_position(this,
        token, "unbalanced parenthesis, expected ')'"));
  case TOKEN_RPAREN:
    (void)reader_next(this);
    mal = lvm_mal_list(this, list);
    return mal;
  default:
    while (TOKEN_RPAREN != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return lvm_mal_error(this, ERROR_READER, text_display_position(this,
              token, "unbalanced parenthesis, expected ')'"));
        } else {
          if (0 == list->count) {
            list_append(this, list, this->constant[CONSTANT_NIL]);
          }
          list_append(this, list, lvm_read_form(this));
          return lvm_mal_list(this, list);
        }
      }
      list_append(this, list, lvm_read_form(this));
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return lvm_mal_error(this, ERROR_READER, text_display_position(this,
            token, "unbalanced parenthesis, expected ')'"));
      }
    }
    token = reader_next(this);
    list_append(this, list, this->constant[CONSTANT_NIL]);
    mal = lvm_mal_list(this, list);
    return mal;
  }
}

mal_p lvm_read_vector(lvm_p this)
{
  return lvm_read_brackets(this);
}

mal_p lvm_read_brackets(lvm_p this)
{
  token_p beginning = reader_peek(this);
  token_p token = reader_next(this);
  vector_p vector = vector_make(this, 0);
  mal_p mal;
  switch (token->type) {
  case TOKEN_EOI:
    printf("nOK%lu %lu\n", token->line, token->column);
    return lvm_mal_error(this, ERROR_READER, text_display_position(this,
        beginning, "unbalanced brackets, expected ']'"));
  case TOKEN_RBRACKET:
    (void)reader_next(this);
    mal = lvm_mal_vector(this, vector);
    return mal;
  default:
    while (TOKEN_RBRACKET != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return lvm_mal_error(this, ERROR_READER, text_display_position(this,
              token, "unbalanced brackets, expected ']'"));
        } else {
          if (0 == vector->count) {
            vector_append(this, vector, this->constant[CONSTANT_NIL]);
          }
          vector_append(this, vector, lvm_read_form(this));
          return lvm_mal_vector(this, vector);
        }
      }
      vector_append(this, vector, lvm_read_form(this));
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
    printf("nOK%lu %lu\n", token->line, token->column);
        return lvm_mal_error(this, ERROR_READER, text_display_position(this,
            beginning, "unbalanced brackets, expected ']'"));
      }
    }
    token = reader_next(this);
    vector_append(this, vector, this->constant[CONSTANT_NIL]);
    mal = lvm_mal_vector(this, vector);
    return mal;
  }
}

mal_p lvm_read_atom(lvm_p this)
{
  token_p token = reader_peek(this);
  reader_next(this);
  switch (token->type) {
  case TOKEN_EOI:
    return lvm_mal_eoi(this);
  case TOKEN_NIL:
    return lvm_mal_nil(this);
  case TOKEN_BOOLEAN:
    if (0 == text_cmp(this, token->as.boolean, "true")) {
      return this->constant[CONSTANT_TRUE];
    } else {
      return this->constant[CONSTANT_FALSE];
    }
  case TOKEN_SYMBOL:
    if (0 == text_cmp(this, token->as.symbol, "nil")) {
      return this->constant[CONSTANT_NIL];
    } else if (0 == text_cmp(this, token->as.symbol, "true")) {
      return this->constant[CONSTANT_TRUE];
    } else if (0 == text_cmp(this, token->as.symbol, "false")) {
      return this->constant[CONSTANT_FALSE];
    } else {
      return lvm_mal_symbol(this, token->as.symbol);
    }
  case TOKEN_KEYWORD:
    return lvm_mal_keyword(this, token->as.keyword);
  case TOKEN_STRING:
    return lvm_mal_string(this, token->as.string);
  case TOKEN_INTEGER:
    return lvm_mal_integer(this, text_to_integer(this, token->as.number));
  case TOKEN_DECIMAL:
    return lvm_mal_decimal(this, text_to_decimal(this, token->as.number));
  default:
    return lvm_mal_error(this, ERROR_READER, text_display_position(this, token,
        "unknown atom type"));
  }
}

mal_p lvm_mal_make(lvm_p this, mal_type type)
{
  mal_p mal = (mal_p)calloc(1, sizeof(mal_t));
  mal->type = type;
  mal->as.nil = NULL;
  mal->token = token_make(this);
  mal->gc.type = GC_MAL;
  mal->gc.next = this->gc.first;
  this->gc.first = (gc_p)mal;
#if GC_ON
  mal->gc.mark = !this->gc.mark;
#else
  mal->gc.mark = this->gc.mark;
#endif
  this->gc.count++;
  return mal;
}

mal_p lvm_mal_eoi(lvm_p this)
{
  mal_p mal = lvm_mal_make(this, MAL_EOI);
  text_p eoi = text_make(this, "eoi");
  mal->as.eoi = eoi;
  mal->token->as.eoi = eoi;
  return mal;
}

mal_p lvm_mal_nil(lvm_p this)
{
  mal_p mal = lvm_mal_make(this, MAL_NIL);
  text_p nil = text_make(this, "nil");
  mal->as.nil = nil;
  mal->token->as.eoi = nil;
  return mal;
}

mal_p lvm_mal_error(lvm_p this, error_type type, text_p text)
{
  mal_p mal = lvm_mal_make(this, MAL_ERROR);
  text_p error;
  switch (type) {
  case ERROR_NONE:
    error = text_concat_text(this, text_make(this, "OK: "), text);
    break;
  case ERROR_READER:
    error = text_concat_text(this, text_make(this, "READER ERROR: "), text);
    break;
  case ERROR_RUNTIME:
    error = text_concat_text(this, text_make(this, "RUNTIME ERROR: "), text);
    break;
  case ERROR_PRINTER:
    error = text_concat_text(this, text_make(this, "PRINTER ERROR: "), text);
    break;
  default:
    error = text_concat_text(this, text_make(this, "UNKNOWN ERROR: "), text);
    break;
  }
  mal->as.error = error;
  mal->token->as.error = error;
  error_append(this, type, text);
  return mal;
}

mal_p lvm_mal_boolean(lvm_p this, bool boolean)
{
  mal_p mal = lvm_mal_make(this, MAL_BOOLEAN);
  text_p text;
  if (boolean) {
    text = text_make(this, "true");
  } else {
    text = text_make(this, "false");
  }
  mal->as.boolean = boolean;
  mal->token->as.boolean = text;
  return mal;
}

mal_p lvm_mal_symbol(lvm_p this, text_p symbol)
{
  mal_p mal = lvm_mal_make(this, MAL_SYMBOL);
  mal->as.symbol = symbol;
  mal->token->as.symbol = symbol;
  return mal;
}

mal_p lvm_mal_keyword(lvm_p this, text_p keyword)
{
  mal_p mal = lvm_mal_make(this, MAL_KEYWORD);
  mal->as.keyword = keyword;
  mal->token->as.keyword = keyword;
  return mal;
}

mal_p lvm_mal_string(lvm_p this, text_p string)
{
  mal_p mal = lvm_mal_make(this, MAL_STRING);
  mal->as.string = string;
  mal->token->as.string = string;
  return mal;
}

mal_p lvm_mal_list(lvm_p this, list_p list)
{
  mal_p mal = lvm_mal_make(this, MAL_LIST);
  mal->as.list = list;
  mal->token->as.list = list_text(this, list);
  return mal;
}

mal_p lvm_mal_vector(lvm_p this, vector_p vector)
{
  mal_p mal = lvm_mal_make(this, MAL_VECTOR);
  mal->as.vector = vector;
  mal->token->as.vector = vector_text(this, vector);
  return mal;
}

mal_p lvm_mal_integer(lvm_p this, long integer)
{
  mal_p mal = lvm_mal_make(this, MAL_INTEGER);
  mal->as.integer = integer;
  mal->token->as.number = text_make_integer(this, integer);
  return mal;
}

mal_p lvm_mal_decimal(lvm_p this, double decimal)
{
  mal_p mal = lvm_mal_make(this, MAL_DECIMAL);
  mal->as.decimal = decimal;
  mal->token->as.number = text_make_decimal(this, decimal);
  return mal;
}

text_p lvm_mal_print(lvm_p this, mal_p mal, bool readable)
{
  text_p text;
  size_t i = 1;

  switch (mal->type) {
  case MAL_EOI:
    return text_make(this, "");
  case MAL_ERROR:
    return mal->as.error;
  case MAL_NIL:
    return text_make(this, "nil");
  case MAL_BOOLEAN:
    if (mal->as.boolean) {
      return text_make(this, "true");
    } else {
      return text_make(this, "false");
    }
  case MAL_LIST:
    text = text_make(this, "(");
    if (mal->as.list->count > 0) {
      text_concat_text(this, text, lvm_mal_print(this, mal->as.list->data[0],
          readable));
      for (; i < (mal->as.list->count - 1); i++) {
        text_append(this, text, ' ');
        text_concat_text(this, text, lvm_mal_print(this, mal->as.list->data[i],
            readable));
      }
      if (MAL_NIL != mal->as.list->data[i]->type) {
        text_concat(this, text, " : ");
        text_concat_text(this, text, lvm_mal_print(this,
            mal->as.list->data[i], readable));
      }
    }
    text_append(this, text, ')');
    return text_append(this, text, 0x00);
  case MAL_VECTOR:
    text = text_make(this, "[");
    if (mal->as.vector->count > 0) {
      text_concat_text(this, text, lvm_mal_print(this, mal->as.vector->data[0],
          readable));
      for (; i < (mal->as.vector->count - 1); i++) {
        text_append(this, text, ' ');
        text_concat_text(this, text, lvm_mal_print(this, mal->as.vector->data[i],
            readable));
      }
      if (MAL_NIL != mal->as.vector->data[i]->type) {
        text_concat(this, text, " : ");
        text_concat_text(this, text, lvm_mal_print(this,
            mal->as.vector->data[i], readable));
      }
    }
    text_append(this, text, ']');
    return text_append(this, text, 0x00);
  case MAL_SYMBOL:
    return mal->as.symbol;
  case MAL_KEYWORD:
    text = text_make(this, ":");
    return text_concat_text(this, text, mal->as.keyword);
  case MAL_STRING:
    if (readable) {
      return text_escape(this, mal->as.string);
    } else {
      text = text_make(this, "\"");
      text_concat_text(this, text, mal->as.string);
      return text_append(this, text, '"');
    }
  case MAL_INTEGER:
    return text_make_integer(this, mal->as.integer);
  case MAL_DECIMAL:
    return text_make_decimal(this, mal->as.decimal);
  default:
    return error_append(this, ERROR_PRINTER,
        text_display_position(this, mal->token, "unknown type of object"));
  }
}

void lvm_mal_free(lvm_p this, gc_p mal)
{
  (void)this;
  free((void *)((mal_p)mal));
}

lvm_p lvm_make()
{
  lvm_p lvm = (lvm_p)calloc(1, sizeof(lvm_t));
  lvm->gc.mark = 0;
  lvm->gc.count = 0;
  lvm->gc.total = GC_TOTAL;
  lvm->gc.first = NULL;
  lvm->error = NULL;
  lvm->comment = NULL;
  lvm->constant[CONSTANT_NIL] = lvm_mal_nil(lvm);
  lvm->constant[CONSTANT_TRUE] = lvm_mal_boolean(lvm, true);
  lvm->constant[CONSTANT_FALSE] = lvm_mal_boolean(lvm, false);
  GC_PROTECT_SET((gc_p)lvm->constant[CONSTANT_NIL]);
  GC_PROTECT_SET((gc_p)lvm->constant[CONSTANT_TRUE]);
  GC_PROTECT_SET((gc_p)lvm->constant[CONSTANT_FALSE]);
  return lvm;
}

void lvm_gc_mark(lvm_p this, gc_p gc)
{
  size_t at;
  if (this->gc.mark == GC_MARK_GET(gc)) {
    return;
  }
  gc->mark = this->gc.mark | GC_PROTECT_GET(gc);
  switch (gc->type) {
  case GC_TEXT:
    break;
  case GC_ERROR:
    for (at = 0; at < ((error_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((error_p)gc)->data[at]));
    }
    break;
  case GC_COMMENT:
    for (at = 0; at < ((comment_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((comment_p)gc)->data[at]));
    }
    break;
  case GC_LIST:
    for (at = 0; at < ((list_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((list_p)gc)->data[at]));
    }
    break;
  case GC_VECTOR:
    for (at = 0; at < ((vector_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((vector_p)gc)->data[at]));
    }
    break;
  case GC_TOKEN:
    break;
  case GC_MAL:
    break;
  }
}

void lvm_gc_mark_all(lvm_p this)
{
  (void)this;
}

void lvm_gc_sweep(lvm_p this)
{
  gc_pp obj = &this->gc.first;
  while (*obj) {
    if (this->gc.mark != GC_MARK_GET(*obj) && !GC_PROTECT_GET(*obj)) {
      gc_p unreached = *obj;
      *obj = unreached->next;
      switch (unreached->type) {
      case GC_TEXT:
        text_free(this, unreached);
        break;
      case GC_ERROR:
        error_free(this, unreached);
        break;
      case GC_COMMENT:
        comment_free(this, unreached);
        break;
      case GC_LIST:
        list_free(this, unreached);
        break;
      case GC_VECTOR:
        vector_free(this, unreached);
        break;
      case GC_TOKEN:
        token_free(this, unreached);
        break;
      case GC_MAL:
        lvm_mal_free(this, unreached);
        break;
      }
      this->gc.count--;
    } else {
      obj = &(*obj)->next;
    }
  }
}

void lvm_gc_print(lvm_p this)
{
#if DEBUG
  size_t i;
  gc_p gc = this->gc.first;
  while (gc) {
    switch (gc->type) {
    case GC_TEXT:
      printf("text: %s\n", ((text_p)gc)->data);
      break;
    case GC_ERROR:
      printf("error: %s\n", error_collapse(this)->data);
      break;
    case GC_COMMENT:
      printf("comment: %s\n", comment_collapse(this)->data);
      break;
    case GC_LIST:
      printf("list: (");
      if (((list_p)gc)->count) {
        printf("%s", lvm_mal_print(this, ((list_p)gc)->data[0], false)->data);
        for (i = 1; i < ((list_p)gc)->count; i++) {
          printf(" %s", lvm_mal_print(this, ((list_p)gc)->data[i],
              false)->data);
        }
        printf(")\n");
      }
      break;
    case GC_VECTOR:
      printf("vector: [");
      if (((vector_p)gc)->count) {
        printf("%s", lvm_mal_print(this, ((vector_p)gc)->data[0],
            false)->data);
        for (i = 1; i < ((vector_p)gc)->count; i++) {
          printf(" %s", lvm_mal_print(this, ((vector_p)gc)->data[i],
              false)->data);
        }
        printf("]\n");
      }
      break;
    case GC_TOKEN:
      printf("token: %s\n", ((token_p)gc)->as.symbol->data);
      break;
    case GC_MAL:
      printf("mal: %s\n", lvm_mal_print(this, ((mal_p)gc), false)->data);
      break;
    default:
      printf("unknown object:\n");
      break;
    }
    gc = gc->next;
  }
#else
  (void)this;
#endif
}

void lvm_gc(lvm_p this)
{
  size_t count = this->gc.count;
#if DEBUG
  lvm_gc_print(this);
#endif
  lvm_gc_mark_all(this);
  lvm_gc_sweep(this);

#if DEBUG
  this->gc.total = this->gc.count == 0 ? GC_TOTAL : this->gc.count * 2;
  printf("Collected %lu objects, %lu remaining.\n", count - this->gc.count,
      this->gc.count);
#else
  (void)count;
#endif
#if GC_ON
  this->gc.mark = !this->gc.mark;
#endif
#if DEBUG
  lvm_gc_print(this);
#endif
}

void lvm_gc_free(lvm_p this)
{
  gc_p gc = this->gc.first;
  gc_p tmp;
  while (gc) {
    tmp = gc;
    gc = gc->next;
    switch (tmp->type) {
    case GC_TEXT:
      text_free(this, tmp);
      break;
    case GC_ERROR:
      error_free(this, tmp);
      break;
    case GC_COMMENT:
      comment_free(this, tmp);
      break;
    case GC_LIST:
      list_free(this, tmp);
      break;
    case GC_VECTOR:
      vector_free(this, tmp);
      break;
    case GC_TOKEN:
      token_free(this, tmp);
      break;
    case GC_MAL:
      lvm_mal_free(this, tmp);
      break;
    }
  }
}

void lvm_free(lvm_pp this)
{
  lvm_gc_free(*this);
  free((void *)(*this));
  (*this) = NULL;
  return;
}

mal_p lvm_read(lvm_p this, char *str)
{
  this->reader.str = str;
  this->reader.pos = 0;
  this->reader.line = 1;
  this->reader.column = 0;
  this->reader.valid[TOKEN_CURRENT] = false;
  this->reader.valid[TOKEN_NEXT] = false;
  error_make(this);
  comment_make(this);
  return lvm_read_str(this);
}

mal_p lvm_eval(lvm_p this, mal_p ast)
{
  (void)this;
  return ast;
}

char *lvm_print(lvm_p this, mal_p value)
{
  char *output = text_str(this, lvm_mal_print(this, value, true));
  if (0 < this->error->count) {
    free((void *)output);
    output = text_str(this, error_collapse(this));
  }
#if DEBUG
  if (0 < this->comment->count) {
    printf("%s\n", comment_collapse(this)->data);
  }
#endif
  return output;
}

char *lvm_rep(lvm_p this, char *str)
{
  return lvm_print(this, lvm_eval(this, lvm_read(this, str)));
}

int main(int argc, char *argv[])
{
  lvm_p lvm = lvm_make();
  (void)argc;
  (void)argv;
  puts("Make-a-lisp version 0.0.3\n");
  puts("Press Ctrl+D to exit\n");
  while (1) {
    char *input = readline(lvm, "mal> ");
    char *output = NULL;
    if (!input) {
      putchar('\n');
      break;
    }
    output = lvm_rep(lvm, input);
    if (0x00 != output[0x00]) {
      printf("%s\n", output);
    } else {
;
#if DEBUG
      printf("\xc2\xab""empty""\xc2\xbb\n");
#endif
    }
    free((void *)output);
    output = NULL;
    free((void *)input);
    lvm_gc(lvm);
  }
  lvm_free(&lvm);
  return 0;
}
