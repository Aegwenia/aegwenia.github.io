## Version 0x1

- The second version of `MAL` project using C89 standard. Reader, writer, lists, vectors, hashmaps ans reader macros. It is equivalent to version 0.0.4.

`gcc -Wpedantic -pedantic -Wall -Wextra -o ./mal_01 ./mal_01.c -lm`

`mal_01.c`
```C
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define GC_ON 1

typedef enum {false, true} bool;

struct gc_s;
typedef struct gc_s gc_t, *gc_p, **gc_pp;
struct text_s;
typedef struct text_s text_t, *text_p, **text_pp;
struct token_s;
typedef struct token_s token_t, *token_p;
struct list_s;
typedef struct list_s list_t, *list_p;
struct vector_s;
typedef struct vector_s vector_t, *vector_p;
struct lvm_s;
typedef struct lvm_s lvm_t, *lvm_p, **lvm_pp;

typedef enum {
  GC_TEXT, GC_TOKEN, GC_LIST, GC_VECTOR
} gc_type;

typedef enum {
  TOKEN_CURRENT, TOKEN_NEXT
} token_position;

typedef enum {
  CONSTANT_NIL, CONSTANT_FALSE, CONSTANT_TRUE
} constant_type;

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

struct list_s {
  gc_t gc;
  text_pp data;
  size_t count;
  size_t capacity;
};

struct vector_s {
  gc_t gc;
  text_pp data;
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
  text_p comment;
  text_p boolean;
  text_p special;
  text_p symbol;
  text_p keyword;
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
  text_p constant[3];
};

text_p text_make(lvm_p this, char* str);
text_p text_append(lvm_p this, text_p text, char item);
text_p text_concat(lvm_p this, text_p text, char *item);
text_p text_concat_text(lvm_p this, text_p text, text_p item);
text_p text_make_integer(lvm_p this, long item);
text_p text_make_decimal(lvm_p this, double item);
text_p text_escape(lvm_p this, text_p text);
text_p text_unescape(lvm_p this, text_p text);
long text_to_integer(lvm_p this, text_p p);
double text_to_decimal(lvm_p this, text_p p);
int text_cmp(lvm_p this, text_p text, char *item);
int text_cmp_text(lvm_p this, text_p text, text_p item);
size_t text_hash_fnv_1a(lvm_p this, text_p p);
size_t text_hash_jenkins(lvm_p this, text_p p);
text_p text_display_position(lvm_p this, token_p token, char *text);
char *text_str(lvm_p this, text_p text);
void text_free(lvm_p this, gc_p text);
list_p list_make(lvm_p this, size_t init);
bool list_append(lvm_p this, list_p list, text_p mal);
void list_free(lvm_p this, gc_p list);
vector_p vector_make(lvm_p this, size_t init);
bool vector_append(lvm_p this, vector_p vector, text_p mal);
void vector_free(lvm_p this, gc_p vector);
char *strdup(char *str);
char *strndup(char *str, size_t n);
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
text_p lvm_read_str(lvm_p this);
text_p lvm_read_form(lvm_p this);
text_p lvm_read_list(lvm_p this);
text_p lvm_read_parenthesis(lvm_p this);
text_p lvm_read_vector(lvm_p this);
text_p lvm_read_brackets(lvm_p this);
text_p lvm_read_atom(lvm_p this);
text_p lvm_mal_boolean(lvm_p this, bool boolean);
text_p lvm_mal_list(lvm_p this, list_p list);
text_p lvm_mal_vector(lvm_p this, vector_p vector);

lvm_p lvm_make();
void lvm_gc(lvm_p this);
void lvm_gc_free(lvm_p this);
void lvm_free(lvm_pp this);
text_p lvm_read(lvm_p this, char *str);
text_p lvm_eval(lvm_p this, text_p ast);
char *lvm_print(lvm_p this, text_p value);
char *lvm_rep(lvm_p this, char *str);

text_p text_make(lvm_p this, char* str)
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
  char *string = text->data;
  size_t index = 1;
  for (; index < text->count; index++) {
    if (0x5C == string[index]) {
      switch (string[index + 1]) {
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
        text_append(this, unescaped, string[index]);
        break;
      }
    } else if (index != text->count && '"' != string[index]) {
      text_append(this, unescaped, string[index]);
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
    } else if ('.' == ch) {
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
  text_make(this, "L:"), text_make_integer(this, token->line)),
  text_concat_text(this, text_append(this, text_concat_text(this,
  text_make(this, " C:"), text_make_integer(this, token->column)), ' '),
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
  list->data = (text_pp)calloc(capacity, sizeof(text_p));
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

bool list_append(lvm_p this, list_p list, text_p mal)
{
  (void)this;
  if (list->count >= list->capacity) {
    text_pp tmp;
    list->capacity <<= 1;
    tmp = (text_pp)realloc(list->data, list->capacity * sizeof(text_p));
    if (NULL == tmp) {
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
  size_t i;
  if (list->count > 0) {
    text_concat_text(this, mal, list->data[0]);
    for (i = 1; i < list->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, list->data[i]);
    }
    if (text_cmp(this, list->data[i], "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, list->data[i]);
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
  vector->data = (text_pp)calloc(capacity, sizeof(text_p));
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

bool vector_append(lvm_p this, vector_p vector, text_p mal)
{
  (void)this;
  if (vector->count >= vector->capacity) {
    text_pp tmp;
    vector->capacity <<= 1;
    tmp = (text_pp)realloc(vector->data, vector->capacity * sizeof(text_p));
    if (NULL == tmp) {
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
    text_concat_text(this, mal, vector->data[0]);
    for (i = 1; i < vector->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, vector->data[i]);
    }
    if (text_cmp(this, vector->data[i], "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, vector->data[i]);
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
  while (isspace(tokenizer_peek_next(this))
      && 0x0A != tokenizer_peek_next(this)) {
    tokenizer_next(this);
  }
  while (0x00 != (ch = tokenizer_peek(this))) {
    switch (ch) {
    case 0x0A:
      text_append(this, text, 0x00);
      token->as.comment = text_display_position(this, token, text->data);
      return token;
    case 0x0D:
      text_append(this, text, 0x00);
      token->as.comment = text_display_position(this, token, text->data);
      return token;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  text_append(this, text, 0x00);

  token->as.comment = text_display_position(this, token, text->data);
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
      if ('.' == ch && !decimal) {
        decimal = true;
        text_append(this, text, ch);
        tokenizer_next(this);
        break;
      } else if ('.' != ch) {
        text_append(this, text, ch);
        tokenizer_next(this);
        break;
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

text_p lvm_read_str(lvm_p this)
{
  if (TOKEN_EOI == reader_peek(this)->type) {
    return text_make(this, "");
  } else {
    return lvm_read_form(this);
  }
}

text_p lvm_read_form(lvm_p this)
{
  token_p token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return text_make(this, "");
  case TOKEN_LPAREN:
    return lvm_read_list(this);
  case TOKEN_RPAREN:
    return text_display_position(this, token,
        "error: unbalanced parenthesis, expected '('");
  case TOKEN_LBRACKET:
    return lvm_read_vector(this);
  case TOKEN_RBRACKET:
    return text_display_position(this, token,
        "error: unbalanced brackets, expected '['");
  default:
    return lvm_read_atom(this);
  }
}

text_p lvm_read_list(lvm_p this)
{
  return lvm_read_parenthesis(this);
}

text_p lvm_read_parenthesis(lvm_p this)
{
  token_p token = reader_next(this);
  list_p list = list_make(this, 0);
  text_p mal;
  token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return text_display_position(this, token,
        "error: unbalanced parenthesis, expected ')'");
  case TOKEN_RPAREN:
    (void)reader_next(this);
    mal = list_text(this, list);
    return mal;
  default:
    while (TOKEN_RPAREN != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return text_display_position(this,
              token, "unbalanced parenthesis, expected ')'");
        } else {
          if (0 == list->count) {
            list_append(this, list, this->constant[CONSTANT_NIL]);
          }
          list_append(this, list, lvm_read_form(this));
          return list_text(this, list);
        }
      }
      list_append(this, list, mal);
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return text_display_position(this, token,
            "error: unbalanced parenthesis, expected ')'");
      }
    }
    token = reader_next(this);
    list_append(this, list, this->constant[CONSTANT_NIL]);
    mal = list_vector(this, list);
    return mal;
  }
}

text_p lvm_read_vector(lvm_p this)
{
  return lvm_read_brackets(this);
}

text_p lvm_read_brackets(lvm_p this)
{
  token_p token = reader_next(this);
  vector_p vector = vector_make(this, 0);
  text_p mal;
  token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return text_display_position(this, token,
        "error: unbalanced brackets, expected ']'");
  case TOKEN_RBRACKET:
    (void)reader_next(this);
    mal = vector_text(this, vector);
    return mal;
  default:
    while (TOKEN_RBRACKET != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return text_display_position(this, token,
              "unbalanced brackets, expected ']'");
        } else {
          if (0 == vector->count) {
            vector_append(this, vector, this->constant[CONSTANT_NIL]);
          }
          vector_append(this, vector, lvm_read_form(this));
          return vector_text(this, vector);
        }
      }
      vector_append(this, vector, mal);
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return text_display_position(this, token,
            "error: unbalanced brackets, expected ']'");
      }
    }
    token = reader_next(this);
    vector_append(this, vector, this->constant[CONSTANT_NIL]);
    mal = vector_text(this, vector);
    return mal;
  }
}

text_p lvm_read_atom(lvm_p this)
{
  token_p token = reader_peek(this);
  text_p mal;
  reader_next(this);
  switch (token->type) {
  case TOKEN_EOI:
    return text_make(this, "eoi");
  case TOKEN_NIL:
    return text_make(this, "nil");
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
      mal = text_make(this, "");
      return text_concat_text(this, mal, token->as.symbol);
    }
  case TOKEN_KEYWORD:
    mal = text_make(this, "");
    return text_concat_text(this, mal, token->as.keyword);
  case TOKEN_STRING:
    mal = text_make(this, "");
    return text_concat_text(this, mal, token->as.string);
  case TOKEN_INTEGER:
    mal = text_make(this, "");
    return text_concat_text(this, mal, token->as.number);
  case TOKEN_DECIMAL:
    mal = text_make(this, "");
    return text_concat_text(this, mal, token->as.number);
  default:
    return text_display_position(this, token,
        "error: unknown atom type");
  }
}

text_p lvm_mal_boolean(lvm_p this, bool boolean)
{
  text_p mal;
  if (boolean) {
    mal = text_make(this, "true");
  } else {
    mal = text_make(this, "false");
  }
  return mal;
}

text_p lvm_mal_list(lvm_p this, list_p list)
{
  text_p mal = text_make(this, "(");
  size_t i = 0;
  if (list->count > 0) {
    text_concat_text(this, mal, list->data[0]);
    for (i = 1; i < list->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, list->data[i]);
    }
    if (text_cmp(this, list->data[i], "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, list->data[i]);
    }
  }
  return text_append(this, mal, ')');
}

text_p lvm_mal_vector(lvm_p this, vector_p vector)
{
  text_p mal = text_make(this, "[");
  size_t i;
  if (vector->count > 0) {
    text_concat_text(this, mal, vector->data[0]);
    for (i = 1; i < vector->count - 1; i++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, vector->data[i]);
    }
    if (text_cmp(this, vector->data[i], "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, vector->data[i]);
    }
  }
  return text_append(this, mal, ']');
}

lvm_p lvm_make()
{
  lvm_p lvm = (lvm_p)calloc(1, sizeof(lvm_t));
  lvm->gc.mark = 0;
  lvm->gc.count = 0;
  lvm->gc.total = 8;
  lvm->gc.first = NULL;
  lvm->constant[CONSTANT_NIL] = text_make(lvm, "nil");
  lvm->constant[CONSTANT_TRUE] = text_make(lvm, "true");
  lvm->constant[CONSTANT_FALSE] = text_make(lvm, "false");
  return lvm;
}

void lvm_gc_mark(lvm_p this, gc_p gc)
{
  size_t at;
  if (this->gc.mark == gc->mark) {
    return;
  }
  gc->mark = this->gc.mark;
  switch (gc->type) {
  case GC_TEXT:
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
  }
}

void lvm_gc_mark_all(lvm_p this)
{
  (void)this;
  this->constant[CONSTANT_NIL]->gc.mark = this->gc.mark;
  this->constant[CONSTANT_TRUE]->gc.mark = this->gc.mark;
  this->constant[CONSTANT_FALSE]->gc.mark = this->gc.mark;
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
      case GC_LIST:
        list_free(this, unreached);
        break;
      case GC_VECTOR:
        vector_free(this, unreached);
        break;
      case GC_TOKEN:
        token_free(this, unreached);
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
  size_t i;
  while (gc) {
    switch (gc->type) {
    case GC_TEXT:
      printf("text: %s\n", ((text_p)gc)->data);
      break;
    case GC_LIST:
      printf("list: (");
      if (((list_p)gc)->count) {
        printf("%s", ((list_p)gc)->data[0]->data);
        for (i = 1; i < ((list_p)gc)->count; i++) {
          printf(" %s", ((list_p)gc)->data[i]->data);
        }
        printf(")\n");
      }
      break;
    case GC_VECTOR:
      printf("vector: [");
      if (((vector_p)gc)->count) {
        printf("%s", ((vector_p)gc)->data[0]->data);
        for (i = 1; i < ((vector_p)gc)->count; i++) {
          printf(" %s", ((vector_p)gc)->data[i]->data);
        }
        printf("]\n");
      }
      break;
    case GC_TOKEN:
      printf("token: %s\n", ((token_p)gc)->as.symbol->data);
      break;
    default:
      printf("unkown object:\n");
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
    case GC_LIST:
      list_free(this, tmp);
      break;
    case GC_VECTOR:
      vector_free(this, tmp);
      break;
    case GC_TOKEN:
      token_free(this, tmp);
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

text_p lvm_read(lvm_p this, char *str)
{
  this->reader.str = str;
  this->reader.pos = 0;
  this->reader.line = 1;
  this->reader.column = 0;
  this->reader.valid[TOKEN_CURRENT] = false;
  this->reader.valid[TOKEN_NEXT] = false;
  return lvm_read_str(this);
}

text_p lvm_eval(lvm_p this, text_p ast)
{
  (void)this;
  return ast;
}

char *lvm_print(lvm_p this, text_p value)
{
  char *output = text_str(this, value);
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
  puts("Make-a-lisp version 0.0.2\n");
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

```
