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

#define GC_ON 1

typedef enum {false, true} bool;

struct gc_s;
typedef struct gc_s gc_t, *gc_p, **gc_pp;
struct text_s;
typedef struct text_s text_t, *text_p, **text_pp;
struct lvm_s;
typedef struct lvm_s lvm_t, *lvm_p, **lvm_pp;

typedef enum {
  GC_TEXT
} gc_type;

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
  } reader;
};

text_p text_make(lvm_p this, char *str);
text_p text_append(lvm_p this, text_p text, char item);
text_p text_concat(lvm_p this, text_p text, char *item);
text_p text_concat_text(lvm_p this, text_p text, text_p item);
text_p text_make_integer(lvm_p this, long item);
text_p text_make_decimal(lvm_p this, double item);
long text_to_integer(lvm_p this, text_p text);
double text_to_decimal(lvm_p this, text_p text);
int text_cmp(lvm_p this, text_p text, char *item);
int text_cmp_text(lvm_p this, text_p text, text_p item);
size_t text_hash_fnv_1a(lvm_p this, text_p text);
size_t text_hash_jenkins(lvm_p this, text_p text);
char *text_str(lvm_p this, text_p text);
void text_free(lvm_p this, gc_p text);
#if __STDC__
#ifndef __STDC_VERSION__
/* C89 */
char *strdup(char *str);
char *strndup(char *str, size_t n);
#else
; /* C90[+] */
#endif
#endif

char *readline(lvm_p this, char *prompt);
lvm_p lvm_make();
void lvm_gc(lvm_p this);
void lvm_gc_free(lvm_p this);
void lvm_free(lvm_pp this);
char *lvm_read(lvm_p this, char *str);
char *lvm_eval(lvm_p this, char *str);
char *lvm_print(lvm_p this, char *str);
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
  } else {
    text->capacity = text->capacity;
  }
  text->count = text->count;
  text->data[text->count] = item;
  if (!item) {
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
  strncpy(text->data + text->count, item, size);
  text->count = text->count + size;
  text->data[text->count] = 0x00;
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
  strncpy(text->data + text->count, item->data, size);
  text->count = text->count + size;
  text->data[text->count] = 0x00;
  return text;
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
    } else if ('.' == ch) {
      decimal = 1;
    }
  }
  return value;
}

int text_cmp(lvm_p this, text_p text, char *item)
{
  (void)this;
  return strncmp(text->data, item, text->count);
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

lvm_p lvm_make()
{
  lvm_p lvm = (lvm_p)calloc(1, sizeof(lvm_t));
  lvm->gc.mark = 0;
  lvm->gc.count = 0;
  lvm->gc.total = 8;
  lvm->gc.first = NULL;
  return lvm;
}

void lvm_gc_mark(lvm_p this, gc_p gc)
{
  if (this->gc.mark == gc->mark) {
    return;
  }
  gc->mark = this->gc.mark;
  switch (gc->type) {
  case GC_TEXT:
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
    if (this->gc.mark != (*obj)->mark) {
      gc_p unreached = *obj;
      *obj = unreached->next;
      switch (unreached->type) {
      case GC_TEXT:
        text_free(this, unreached);
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
  gc_p gc = this->gc.first;
  while (gc) {
    switch (gc->type) {
    case GC_TEXT:
      printf("text: %s\n", ((text_p)gc)->data);
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
#if DEBUG
  size_t count = this->gc.count;
#endif
  lvm_gc_mark_all(this);
  lvm_gc_sweep(this);

  this->gc.total = this->gc.count == 0 ? 8 : this->gc.count * 2;
#if DEBUG
  printf("Collected %lu objects, %lu remaining.\n", count - this->gc.count,
      this->gc.count);
#endif
#if GC_ON
  this->gc.mark = !this->gc.mark;
#endif

#if DEBUG
  lvm_gc_print(this);
#endif
}

void lvm_free(lvm_pp this)
{
  lvm_gc(*this);
  free((void *)(*this));
  (*this) = NULL;
  return;
}

char *lvm_read(lvm_p this, char *str)
{
  this->reader.str = str;
  this->reader.pos = 0;
  return str;
}

char *lvm_eval(lvm_p this, char *str)
{
  (void)this;
  return str;
}

char *lvm_print(lvm_p this, char *str)
{
  char *output = text_str(this, text_make(this, str));
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
  puts("Make-a-lisp version 0.0.1\n");
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
    }
    free((void *)output);
    output = NULL;
    free((void *)input);
    lvm_gc(lvm);
  }
  lvm_free(&lvm);
  return 0;
}
