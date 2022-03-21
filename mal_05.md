## Version 0x05 -- TCO (Tail Call Optimization); version up 
[***Main Page***](./README.md)
[***\[edit\]***](https://github.com/Aegwenia/aegwenia.github.io/edit/main/mal_05.md)

- The sixth version of `MAL` project using C89 standard. Editing eval functions to support TCO (Tail Call Optimization).

`gcc -Wpedantic -pedantic -Wall -Wextra -o ./mal_05 ./mal_05.c -lm`

[***./src/mal_05.c***](./src/mal_05.c)
```C
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define GC_ON 1
#define VAR_NIL 0

typedef enum {false, true} bool;

struct gc_s;
typedef struct gc_s gc_t, *gc_p, **gc_pp;
struct text_s;
typedef struct text_s text_t, *text_p, **text_pp;
struct token_s;
typedef struct token_s token_t, *token_p;
struct function_s;
typedef struct function_s function_t, *function_p;
struct closure_s;
typedef struct closure_s closure_t, *closure_p;
struct list_s;
typedef struct list_s list_t, *list_p;
struct vector_s;
typedef struct vector_s vector_t, *vector_p;
struct hashmap_s;
typedef struct hashmap_s hashmap_t, *hashmap_p;
struct env_s;
typedef struct env_s env_t, *env_p, **env_pp;
struct error_s;
typedef struct error_s error_t, *error_p;
struct comment_s;
typedef struct comment_s comment_t, *comment_p;
struct mal_s;
typedef struct mal_s mal_t, *mal_p, **mal_pp;
struct lvm_s;
typedef struct lvm_s lvm_t, *lvm_p, **lvm_pp;

typedef enum {
  GC_TEXT, GC_TOKEN, GC_LIST, GC_VECTOR, GC_ENV, GC_HASHMAP, GC_MAL, GC_COMMENT,
  GC_FUNCTION, GC_CLOSURE, GC_ERROR
} gc_type;

typedef enum {
  TOKEN_CURRENT, TOKEN_NEXT
} token_position;

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

struct function_s {
  gc_t gc;
  mal_p (*definition)(lvm_p this, mal_p params);
  text_p name;
};

struct closure_s {
  gc_t gc;
  env_p env;
  mal_p parameters;
  mal_p more;
  mal_p definition;
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

struct hashmap_s {
  gc_t gc;
  mal_pp data;
  size_t count;
  size_t capacity;
};

struct env_s {
  gc_t gc;
  env_p outer;
  vector_p hashmap;
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

typedef enum {
  TOKEN_EOI, TOKEN_NIL, TOKEN_COMMENT, TOKEN_BOOLEAN, TOKEN_QUOTE, TOKEN_COLON,
  TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_AT, TOKEN_LBRACKET, TOKEN_BACKSLASH,
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
  text_p string;
  text_p function;
  text_p closure;
  text_p list;
  text_p vector;
  text_p hashmap;
  text_p env;
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
  MAL_NIL, MAL_LIST, MAL_VECTOR, MAL_HASHMAP, MAL_INTEGER, MAL_DECIMAL,
  MAL_ENV, MAL_FUNCTION, MAL_CLOSURE
} mal_type;

typedef union {
  text_p eoi;
  text_p error;
  bool boolean;
  text_p symbol;
  text_p keyword;
  text_p string;
  text_p nil;
  function_p function;
  closure_p closure;
  list_p list;
  vector_p vector;
  hashmap_p hashmap;
  env_p env;
  long integer;
  double decimal;
} mal_value;

struct mal_s {
  gc_t gc;
  mal_type type;
  mal_value as;
  token_p token;
  text_p signature;
  text_p identity;
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
  env_p env;
  error_p error;
  comment_p comment;
};

text_p text_make(lvm_p this, char *str);
text_p text_append(lvm_p this, text_p text, char item);
text_p text_concat(lvm_p this, text_p text, char *item);
text_p text_concat_text(lvm_p this, text_p text, text_p item);
text_p text_offset(lvm_p this, text_p text, size_t offset);
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
function_p function_make(lvm_p this,
    mal_p (*definition)(lvm_p this, mal_p params), text_p name);
void function_free(lvm_p this, gc_p gc);
closure_p closure_make(lvm_p this, env_p env, mal_p parameters,
    mal_p definition, mal_p more);
text_p closure_text(lvm_p this, closure_p closure);
mal_p closure_parameters(lvm_p this, mal_pp params, mal_pp more);
void closure_free(lvm_p this, gc_p gc);
list_p list_make(lvm_p this, size_t init);
bool list_append(lvm_p this, list_p list, mal_p mal);
text_p list_text(lvm_p this, list_p list);
text_p list_more_text(lvm_p this, list_p list, mal_p more);
list_p list_offset(lvm_p this, list_p original, size_t offset);
list_p list_params(lvm_p this, list_p original);
mal_p list_find(lvm_p this, list_p list, mal_p symbol);
mal_p list_equal(lvm_p this, list_p list0, list_p list1);
void list_free(lvm_p this, gc_p list);
vector_p vector_make(lvm_p this, size_t init);
bool vector_append(lvm_p this, vector_p vector, mal_p mal);
mal_p vector_equal(lvm_p this, vector_p vector0, vector_p vector1);
void vector_free(lvm_p this, gc_p vector);
hashmap_p hashmap_make(lvm_p this, size_t init);
bool hashmap_set(lvm_p this, hashmap_p hashmap, mal_p key, mal_p value);
bool hashmap_get(lvm_p this, hashmap_p hashmap, mal_p key, mal_pp value);
bool hashmap_get_by_text(lvm_p this, hashmap_p hashmap, text_p key,
    mal_pp value);
bool hashmap_append(lvm_p this, hashmap_p hashmap, mal_p mal);
text_p hashmap_text(lvm_p this, hashmap_p hashmap);
text_p hashmap_text(lvm_p this, hashmap_p hashmap);
mal_p hashmap_equal(lvm_p this, hashmap_p hashmap0, hashmap_p hashmap1);
void hashmap_free(lvm_p this, gc_p hashmap);
env_p env_make(lvm_p this, env_p outer, list_p symbols, list_p exprs,
    mal_p more, size_t init);
bool env_set(lvm_p this, env_p env, mal_p key, mal_p value);
bool env_get(lvm_p this, env_p env, mal_p key, mal_pp value);
bool env_get_by_text(lvm_p this, env_p env, text_p key, mal_pp value);
text_p env_text(lvm_p this, env_p env);
void env_free(lvm_p this, gc_p env);
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
mal_p read_str(lvm_p this);
mal_p read_form(lvm_p this);
mal_p read_list(lvm_p this);
mal_p read_parenthesis(lvm_p this);
mal_p read_vector(lvm_p this);
mal_p read_brackets(lvm_p this);
mal_p read_hashmap(lvm_p this);
mal_p read_braces(lvm_p this);
mal_p read_symbol_list(lvm_p this, char *name);
mal_p read_atom(lvm_p this);
mal_p mal_make(lvm_p this, mal_type type);
mal_p mal_eoi(lvm_p this);
mal_p mal_nil(lvm_p this);
mal_p mal_error(lvm_p this, error_type type, text_p text);
mal_p mal_boolean(lvm_p this, bool boolean);
mal_p mal_symbol(lvm_p this, text_p symbol);
mal_p mal_keyword(lvm_p this, text_p keyword);
mal_p mal_string(lvm_p this, text_p string);
mal_p mal_list(lvm_p this, list_p list);
mal_p mal_vector(lvm_p this, vector_p vector);
mal_p mal_hashmap(lvm_p this, hashmap_p hashmap);
mal_p mal_integer(lvm_p this, long integer);
mal_p mal_decimal(lvm_p this, double decimal);
mal_p mal_as_str(lvm_p this, mal_p args, bool readable, char *separator);
text_p mal_print(lvm_p this, mal_p mal, bool readable);
void mal_free(lvm_p this, gc_p mal);
bool is_eoi(mal_p mal);
bool is_nil(mal_p mal);
bool is_true(mal_p mal);
bool is_false(mal_p mal);
bool is_error(mal_p mal);
bool is_integer(mal_p mal);
bool is_decimal(mal_p mal);
bool is_number(mal_p mal);
bool is_symbol(mal_p mal);
bool is_keyword(mal_p mal);
bool is_string(mal_p mal);
bool is_self_evaluating(mal_p mal);
bool is_list(mal_p mal);
bool is_vector(mal_p mal);
bool is_hashmap(mal_p mal);
bool is_env(mal_p mal);
bool is_sequential(mal_p mal);
bool is_function(mal_p mal);
bool is_callable(mal_p mal);
lvm_p lvm_make();
void lvm_gc(lvm_p this);
void lvm_gc_free(lvm_p this);
void lvm_free(lvm_pp this);
mal_p eval_ast(lvm_p this, mal_p ast, env_p env);
mal_p eval_list(lvm_p this, list_p original, env_p env);
mal_p eval_vector(lvm_p this, vector_p original, env_p env);
mal_p eval_hashmap(lvm_p this, hashmap_p original, env_p env);
mal_p eval_def_bang(lvm_p this, mal_p ast, env_p env);
mal_p eval_let_star(lvm_p this, mal_p ast, env_pp outer);
mal_p eval_if(lvm_p this, mal_p ast, env_pp env);
mal_p eval_fn_star(lvm_p this, mal_p ast, env_p env);
mal_p eval_do(lvm_p this, mal_p ast, env_p env);
mal_p core_add(lvm_p this, mal_p args);
mal_p core_sub(lvm_p this, mal_p args);
mal_p core_mul(lvm_p this, mal_p args);
mal_p core_div(lvm_p this, mal_p args);
mal_p core_eq(lvm_p this, mal_p args);
mal_p core_lt(lvm_p this, mal_p args);
mal_p core_le(lvm_p this, mal_p args);
mal_p core_gt(lvm_p this, mal_p args);
mal_p core_ge(lvm_p this, mal_p args);
mal_p core_list(lvm_p this, mal_p args);
mal_p core_vector(lvm_p this, mal_p args);
mal_p core_hashmap(lvm_p this, mal_p args);
mal_p core_zip(lvm_p this, mal_p args);
mal_p core_listp(lvm_p this, mal_p args);
mal_p core_vectorp(lvm_p this, mal_p args);
mal_p core_hashmapp(lvm_p this, mal_p args);
mal_p core_envp(lvm_p this, mal_p args);
mal_p core_emptyp(lvm_p this, mal_p args);
mal_p core_count(lvm_p this, mal_p args);
mal_p core_pr_str(lvm_p this, mal_p args);
mal_p core_str(lvm_p this, mal_p args);
mal_p core_prn(lvm_p this, mal_p args);
mal_p core_println(lvm_p this, mal_p args);
mal_p lvm_read(lvm_p this, char *str);
mal_p lvm_eval(lvm_p this, mal_p ast, env_p env);
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
    text->data = (char *)realloc(text->data, (text->capacity + 1) *
        sizeof(char));
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

text_p text_offset(lvm_p this, text_p text, size_t offset)
{
  text_p offseted = text_make(this, "");
  size_t at;
  for (at = offset; at < text->count; at++) {
    text_append(this, offseted, text->data[at]);
  }
  return offseted;
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
    int from;
    int to, tmp;
    if (0 > integer) {
      text_append(this, result, '-');
      integer *= -1;
    }
    from = result->count;
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
    while (fractional > 0 && digits++ >= 16) {
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
  size = text->count > size ? text->count: size;
  (void)this;
  return strncmp(text->data, item, size);
}

int text_cmp_text(lvm_p this, text_p text, text_p item)
{
  size_t size = text->count > item->count ? text->count: item->count;
  (void)this;
  return strncmp(text->data, item->data, size);
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

char text_get(lvm_p this, text_p text, size_t offset)
{
  (void)this;
  if (offset < text->count) {
    return text->data[offset];
  } else {
    return 0x00;
  }
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

function_p function_make(lvm_p this,
    mal_p (*definition)(lvm_p this, mal_p params), text_p name)
{
  function_p function = (function_p)calloc(1, sizeof(function_t));
  function->definition = definition;
  function->name = name;
  function->gc.type = GC_FUNCTION;
#if GC_ON
  function->gc.mark = !this->gc.mark;
#else
  function->gc.mark = this->gc.mark;
#endif
  function->gc.next = this->gc.first;
  this->gc.first = (gc_p)function;
  this->gc.count++;
  return function;
}

void function_free(lvm_p this, gc_p gc)
{
  (void)this;
  free((void *)gc);
}

closure_p closure_make(lvm_p this, env_p env, mal_p parameters,
    mal_p definition, mal_p more)
{
  closure_p closure = (closure_p)calloc(1, sizeof(closure_t));
  closure->env = env;
  closure->parameters = parameters;
  closure->definition = definition;
  closure->more = more;
  closure->gc.type = GC_CLOSURE;
#if GC_ON
  closure->gc.mark = !this->gc.mark;
#else
  closure->gc.mark = this->gc.mark;
#endif
  closure->gc.next = this->gc.first;
  this->gc.first = (gc_p)closure;
  this->gc.count++;
  return closure;
}

text_p closure_text(lvm_p this, closure_p closure)
{
  text_p mal = text_make(this, "(fn* ");
  text_concat_text(this, mal, list_more_text(this, closure->parameters->as.list,
      closure->more));
  text_append(this, mal, ' ');
  text_concat_text(this, mal, mal_print(this, closure->definition, false));
  return text_append(this, mal, ')');
}

mal_p closure_parameters(lvm_p this, mal_pp params, mal_pp more)
{
  mal_p nil;
  list_p list = (*params)->as.list;
  list_p args = list_make(this, list->count - 1);
  size_t at;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  (*more) = nil;
  if (!is_list(*params) && is_symbol(*params)) {
    args = list_make(this, 1);
    list_append(this, args, (*params));
    list_append(this, args, nil);
    (*params) = mal_list(this, args);
    return nil;
  } else if (!is_list(*params) && !is_symbol(*params)) {
      return mal_error(this, ERROR_RUNTIME, text_concat(this,
          text_concat_text(this, text_make(this,
          "'fn*': non-symbol in argument list '"),
          mal_print(this, *params, false)), "'\n"));
  }
  for (at = 0; at < list->count - 1; at++) {
    mal_p mal = list->data[at];
    if (!is_symbol(mal)) {
      return mal_error(this, ERROR_RUNTIME, text_concat(this,
          text_concat_text(this, text_make(this,
          "'fn*': non-symbol in argument list '"), mal_print(this, mal, false)),
          "'\n"));
    }
    if ('&' == mal->as.symbol->data[0]) {
      if (0x00 == mal->as.symbol->data[1]) {
        if (is_nil(list->data[at + 1])) {
          return mal_error(this, ERROR_RUNTIME, text_make(this,
              "'fn*': missing symbol after '&' in argument list\n"));
        } else if (is_symbol(list->data[at + 1]) &&
            is_nil(list->data[at + 2]) && (at + 3) == list->count) {
          if (NULL != list_find(this, args, mal)) {
            return mal_error(this, ERROR_RUNTIME, text_concat(this,
                text_concat_text(this, text_make(this,
                "'fn*': more symbol is duplicate symbol in argument list '"),
                mal_print(this, mal, false)), "'\n"));
          } else {
            (*more) = list->data[at + 1];
            break;
          }
        } else if (is_symbol(list->data[at + 1]) &&
            !is_nil(list->data[at + 2])) {
          return mal_error(this, ERROR_RUNTIME, text_concat(this,
              text_concat_text(this, text_concat(this, text_concat_text(this,
              text_make(this, "'fn*': unexpected symbol after'& "),
              mal_print(this, list->data[at + 1], false)),
              "' symbol in argument list: '"), mal_print(this, mal, false)),
              "'\n"));
        }
      } else {
        if (is_nil(list->data[at + 1]) && (at + 2) == list->count) {
          (*more) = mal_symbol(this, text_offset(this, mal->as.symbol, 1));
        } else {
          return mal_error(this, ERROR_RUNTIME, text_concat(this,
              text_concat_text(this, text_concat(this, text_concat_text(this,
              text_make(this, "'fn*': unexpected symbol after '"),
              mal_print(this, list->data[at + 1], false)),
              "' symbol in argument list: '"), mal_print(this, mal, false)),
              "'\n"));
        }
      }
    } else {
      if (NULL != list_find(this, args, mal)) {
        return mal_error(this, ERROR_RUNTIME, text_concat(this,
            text_concat_text(this, text_make(this,
            "'fn*': duplicate symbol in argument list '"),
            mal_print(this, mal, false)), "'\n"));
      } else {
        list_append(this, args, mal);
      }
    }
  }
  list_append(this, args, nil);
  (*params) = mal_list(this, args);
  return nil;
}

void closure_free(lvm_p this, gc_p gc)
{
  (void)this;
  free((void *)gc);
}

list_p list_make(lvm_p this, size_t init)
{
  list_p list = (list_p)calloc(1, sizeof(list_t));
  size_t capacity = 2;
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
  size_t at = 0;
  if (list->count > 0) {
    text_concat_text(this, mal, mal_print(this, list->data[0], false));
    for (at = 1; at < list->count - 1; at++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, list->data[at], false));
    }
    if (1 != at && text_cmp(this, mal_print(this, list->data[at],
        false), "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, mal_print(this, list->data[at], false));
    }
  }
  return text_append(this, mal, ')');
}

text_p list_more_text(lvm_p this, list_p list, mal_p more)
{
  text_p mal = text_make(this, "(");
  size_t at = 0;
  if (list->count > 0) {
    text_concat_text(this, mal, mal_print(this, list->data[0], false));
    for (at = 1; at < list->count - 1; at++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, list->data[at], false));
    }
    if (!is_nil(list->data[at])) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, list->data[at], false));
    }
  }
  if (!is_nil(more)) {
    text_concat(this, mal, " & ");
    text_concat_text(this, mal, mal_print(this, more, false));
  }
  return text_append(this, mal, ')');
}

list_p list_offset(lvm_p this, list_p original, size_t offset)
{
  list_p offseted = list_make(this,
      ((long)original->count - offset) < original->count ?
      (original->count - offset) : 0);
  size_t at;
  mal_p mal;

  for (at = offset; at < original->count; at++) {
    list_append(this, offseted, original->data[at]);
  }
  if (0 == offseted->count) {
    env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
    list_append(this, offseted, mal);
  }
  return offseted;
}

list_p list_params(lvm_p this, list_p original)
{
  list_p params = list_make(this,
      ((long)original->count - 1) >= 0 ? (original->count - 1) : 0);
  size_t at;
  mal_p mal;

  for (at = 1; at < original->count; at++) {
    list_append(this, params, original->data[at]);
  }
  if (0 == params->count) {
    env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
    list_append(this, params, mal);
  }
  return params;
}

mal_p list_get(lvm_p this, list_p list, size_t offset)
{
  (void)this;
  if (offset < list->count) {
    return list->data[offset];
  } else {
    return NULL;
  }
}

mal_p list_find(lvm_p this, list_p list, mal_p symbol)
{
  mal_p result = NULL;
  text_p text = symbol->as.symbol;
  size_t at;
  for (at = 0; at < list->count; at++) {
    if (0 == text_cmp_text(this, text, list->data[at]->as.symbol)) {
      result = list->data[at];
      return result;
    }
  }
  return result;
}

mal_p list_equal(lvm_p this, list_p list0, list_p list1)
{
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (list0->count != list1->count) {
    return f;
  } else {
    size_t at;
    for (at = 0; at < list0->count; at++) {
      list_p args = list_make(this, 0);
      mal_p cmp;
      list_append(this, args, list0->data[at]);
      list_append(this, args, list1->data[at]);
      cmp = core_eq(this, mal_list(this, args));
      if (is_false(cmp)) {
        return f;
      }
    }
    return t;
  }
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
  size_t at;
  if (vector->count > 0) {
    text_concat_text(this, mal, mal_print(this, vector->data[0], false));
    for (at = 1; at < vector->count - 1; at++) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, vector->data[at], false));
    }
    if (1 != at && text_cmp(this, mal_print(this, vector->data[at], false),
        "nil")) {
      text_concat(this, mal, " : ");
      text_concat_text(this, mal, mal_print(this, vector->data[at], false));
    }
  }
  return text_append(this, mal, ']');
}

mal_p vector_get(lvm_p this, vector_p vector, size_t offset)
{
  (void)this;
  if (offset < vector->count) {
    return vector->data[offset];
  } else {
    return NULL;
  }
}

mal_p vector_equal(lvm_p this, vector_p vector0, vector_p vector1)
{
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (vector0->count != vector1->count) {
    return f;
  } else {
    size_t at;
    for (at = 0; at < vector0->count; at++) {
      list_p args = list_make(this, 0);
      mal_p cmp;
      list_append(this, args, vector0->data[at]);
      list_append(this, args, vector1->data[at]);
      cmp = core_eq(this, mal_list(this, args));
      if (is_false(cmp)) {
        return f;
      }
    }
    return t;
  }
}

void vector_free(lvm_p this, gc_p vector)
{
  (void)this;
  free((void *)((vector_p)vector)->data);
  free((void *)vector);
}

hashmap_p hashmap_make(lvm_p this, size_t init)
{
  hashmap_p hashmap = (hashmap_p)calloc(1, sizeof(hashmap_t));
  size_t capacity = 2;
  if (0 == init) {
    init = 2;
  }
  while (capacity < init) {
    capacity = (capacity << 1);
  }
  hashmap->count = 0;
  hashmap->capacity = capacity;
  hashmap->data = (mal_pp)calloc(capacity, sizeof(mal_p));
  hashmap->gc.type = GC_HASHMAP;
#if GC_ON
  hashmap->gc.mark = !this->gc.mark;
#else
  hashmap->gc.mark = this->gc.mark;
#endif
  hashmap->gc.next = this->gc.first;
  this->gc.first = (gc_p)hashmap;
  this->gc.count++;
  return hashmap;
}

bool hashmap_set(lvm_p this, hashmap_p hashmap, mal_p key, mal_p value)
{
  size_t at = 0;
  for  (at = 0; at < hashmap->count; at = at + 2) {
    if (0 == text_cmp_text(this, hashmap->data[at]->signature, key->signature)
        && hashmap->count >= at+1) {
      hashmap->data[at+1] = value;
      return true;
    }
  }
  hashmap_append(this, hashmap, key);
  hashmap_append(this, hashmap, value);
  return true;
}

bool hashmap_get(lvm_p this, hashmap_p hashmap, mal_p key, mal_pp value)
{
  size_t at = 0;
  *value = NULL;
  for  (at = 0; at < hashmap->count; at = at + 2) {
    if (0 == text_cmp_text(this, hashmap->data[at]->signature, key->signature)) {
      *value = hashmap->data[at+1];
      return true;
    }
  }
  return false;
}

bool hashmap_get_by_text(lvm_p this, hashmap_p hashmap, text_p key, mal_pp value)
{
  size_t at = 0;
  *value = NULL;
  for  (at = 0; at < hashmap->count; at = at + 2) {
    if (0 == text_cmp_text(this, hashmap->data[at]->signature, key)) {
      *value = hashmap->data[at+1];
      return true;
    }
  }
  
  return false;
}

bool hashmap_append(lvm_p this, hashmap_p hashmap, mal_p mal)
{
  (void)this;
  if (hashmap->count >= hashmap->capacity) {
    mal_pp tmp;
    hashmap->capacity <<= 1;
    tmp = (mal_pp)realloc(hashmap->data, hashmap->capacity * sizeof(mal_p));
    if (NULL == tmp) {
      error_append(this, ERROR_RUNTIME, text_display_position(this,
          reader_peek(this), "not enough memory"));
      return false;
    }
    hashmap->data = tmp;
  }
  hashmap->data[hashmap->count++] = mal;
  return true;
}

text_p hashmap_text(lvm_p this, hashmap_p hashmap)
{
  text_p mal = text_make(this, "{");
  size_t i;
  if (hashmap->count > 0) {
    text_concat_text(this, mal, mal_print(this, hashmap->data[0], false));
    text_concat(this, mal, ": ");
    text_concat_text(this, mal, mal_print(this, hashmap->data[1], false));
    for (i = 2; i < hashmap->count; i += 2) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, hashmap->data[i], false));
      text_concat(this, mal, ": ");
      text_concat_text(this, mal, mal_print(this, hashmap->data[i + 1], false));
    }
  }
  return text_append(this, mal, '}');
}

mal_p hashmap_equal(lvm_p this, hashmap_p hashmap0, hashmap_p hashmap1)
{
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (0 == hashmap0->count && 0 == hashmap1->count) {
    return t;
  }
  if (hashmap0->count != hashmap1->count) {
    return f;
  } else {
    size_t at;
    for (at = 0; at < hashmap0->count; at += 2) {
      mal_p cmp;
      mal_p key0 = hashmap0->data[at];
      mal_p value0 = hashmap0->data[at + 1];
      mal_p value1 = NULL;
      list_p args = list_make(this, 0);
      hashmap_get(this, hashmap1, key0, &value1);
      if (!value1) {
        return f;
      }
      list_append(this, args, value0);
      list_append(this, args, value1);
      cmp = core_eq(this, mal_list(this, args));
      if (is_false(cmp)) {
        return f;
      }
    }
    return t;
  }
}

void hashmap_free(lvm_p this, gc_p hashmap)
{
  (void)this;
  free((void *)((hashmap_p)hashmap)->data);
  free((void *)hashmap);
}

env_p env_make(lvm_p this, env_p outer, list_p symbols, list_p exprs,
    mal_p more, size_t init)
{
  env_p env = (env_p)calloc(1, sizeof(env_t));
  size_t capacity = 2;
  size_t at = 0;
  if (0 == init) {
    init = 2;
  }
  while (capacity < init) {
    capacity = (capacity << 1);
  }
  env->outer = outer;
  env->hashmap = vector_make(this, init);
  env->gc.type = GC_ENV;
#if GC_ON
  env->gc.mark = !this->gc.mark;
#else
  env->gc.mark = this->gc.mark;
#endif
  env->gc.next = this->gc.first;
  this->gc.first = (gc_p)env;
  this->gc.count++;
  if (symbols) {
    for (; at < symbols->count - 1; at++) {
      env_set(this, env, list_get(this, symbols, at),
          list_get(this, exprs, at));
    }
  }
  if (more && !is_nil(more)) {
    env_set(this, env, more, list_get(this, exprs, at));
  }
  return env;
}

bool env_set(lvm_p this, env_p env, mal_p key, mal_p value)
{
  size_t at = 0;
  for (at = 0; at < env->hashmap->count; at = at + 2) {
    if (0 == text_cmp_text(this, env->hashmap->data[at]->signature,
        key->signature) && env->hashmap->count >= at + 1) {
      env->hashmap->data[at+1] = value;
      return true;
    }
  }
  vector_append(this, env->hashmap, key);
  vector_append(this, env->hashmap, value);
  return true;
}

bool env_get(lvm_p this, env_p env, mal_p key, mal_pp value)
{
  size_t at = 0;
  (*value) = NULL;
  while (env) {
    for  (at = 0; at < env->hashmap->count; at = at + 2) {
      if (0 == text_cmp_text(this, env->hashmap->data[at]->signature,
          key->signature)) {
        (*value) = env->hashmap->data[at+1];
        return true;
      }
    }
    env = env->outer;
  }
  return false;
}

bool env_get_by_text(lvm_p this, env_p env, text_p key, mal_pp value)
{
  size_t at = 0;
  (*value) = NULL;
  while (env) {
    for  (at = 0; at < env->hashmap->count; at = at + 2) {
      if (0 == text_cmp_text(this, env->hashmap->data[at]->signature, key)) {
        (*value) = env->hashmap->data[at+1];
        return true;
      }
    }
    env = env->outer;
  }
  return false;
}

text_p env_text(lvm_p this, env_p env)
{
  text_p mal = text_make(this, "{");
  size_t i;
  if (env->hashmap->count > 0) {
    text_concat_text(this, mal, mal_print(this, env->hashmap->data[0], false));
    text_concat(this, mal, ": ");
    text_concat_text(this, mal, mal_print(this, env->hashmap->data[1], false));
    for (i = 2; i < env->hashmap->count; i += 2) {
      text_append(this, mal, ' ');
      text_concat_text(this, mal, mal_print(this, env->hashmap->data[i],
          false));
      text_concat(this, mal, ": ");
      text_concat_text(this, mal, mal_print(this, env->hashmap->data[i + 1],
          false));
    }
  }
  return text_append(this, mal, '}');
}

void env_free(lvm_p this, gc_p env)
{
  (void)this;
  free((void *)env);
}

char *strdup(char *str)
{
  char *result;
  char *p = str;
  size_t n = 0;

  while (*p++) {
    n++;
  }
  result = malloc(n * sizeof(char) + 1);
  p = result;
  while (*str) {
    (*p++) = *str++;
  }
  (*p) = 0x00;
  return result;
}

char *strndup(char *str, size_t n)
{
  char *result;
  char *p;
  result = malloc(n * sizeof(char) + 1);
  p = result;
  while (*str) {
    (*p++) = *str++;
  }
  (*p) = 0x00;
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
    while (isspace(ch = tokenizer_peek(this)) || ',' == ch) {
      switch (ch) {
      case 0x09:
      case 0x0A:
      case 0x0D:
      case 0x20:
      case ',':
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
      switch (tokenizer_peek_next(this)) {
      case 0x09:
      case 0x0A:
      case 0x0D:
      case ' ':
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
      default:
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
  token->line = this->reader.line;
  token->column = this->reader.column;
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
      token->line = this->reader.line;
      token->column = this->reader.column;
      token->as.comment = text_display_position(this, token, text->data);
      comment_append(this, token->as.comment);
      return token;
    case 0x0D:
      token->line = this->reader.line;
      token->column = this->reader.column;
      token->as.comment = text_display_position(this, token, text->data);
      comment_append(this, token->as.comment);
      return token;
    default:
      text_append(this, text, tokenizer_peek(this));
      break;
    }
    tokenizer_next(this);
  }
  token->line = this->reader.line;
  token->column = this->reader.column;
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
    token->type = TOKEN_BACKSLASH;
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
  token->as.special = text;
  token->line = this->reader.line;
  token->column = this->reader.column;
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
        token->line = this->reader.line;
        token->column = this->reader.column;
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
      token->line = this->reader.line;
      token->column = this->reader.column;
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
  token->line = this->reader.line;
  token->column = this->reader.column;
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
      token->line = this->reader.line;
      token->column = this->reader.column;
      return token;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.symbol = text;
  token->line = this->reader.line;
  token->column = this->reader.column;
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
      token->line = this->reader.line;
      token->column = this->reader.column;
      return token;
    default:
      text_append(this, text, tokenizer_next(this));
      break;
    }
  }
  text_append(this, text, 0x00);
  token->length = text->count;
  token->as.keyword = text;
  token->line = this->reader.line;
  token->column = this->reader.column;
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
      token->line = this->reader.line;
      token->column = this->reader.column;
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
  token->length = text->count;
  token->as.string = text;
  token->line = this->reader.line;
  token->column = this->reader.column;
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

mal_p read_str(lvm_p this)
{
  if (TOKEN_EOI == reader_peek(this)->type) {
    return mal_eoi(this);
  } else if (0 < this->error->count) {
    return mal_eoi(this);
  } else {
    return read_form(this);
  }
}

mal_p read_form(lvm_p this)
{
  token_p token = reader_peek(this);
  list_p list = list_make(this, 0);
  mal_p mal;
  mal_p first;
  mal_p second;
  while (true) {
    switch (token->type) {
    case TOKEN_EOI:
      return mal_eoi(this);
    case TOKEN_QUOTE:
      return read_symbol_list(this, "quote");
    case TOKEN_LPAREN:
      return read_list(this);
    case TOKEN_RPAREN:
      return mal_error(this, ERROR_READER, text_display_position(this, token,
          "unbalanced parenthesis, expected '('"));
    case TOKEN_COLON:
      return mal_error(this, ERROR_READER, text_display_position(this, token,
          "unexpected colon character ':'"));
    case TOKEN_AT:
      return read_symbol_list(this, "deref");
    case TOKEN_LBRACKET:
      return read_vector(this);
    case TOKEN_BACKSLASH:
      return mal_error(this, ERROR_READER, text_display_position(this, token,
          "unexpected backslash character '\\'"));
    case TOKEN_RBRACKET:
      return mal_error(this, ERROR_READER, text_display_position(this, token,
            "unbalanced brackets, expected '['"));
    case TOKEN_CARET:
      reader_next(this);
      list_append(this, list, mal_symbol(this, text_make(this,
          "with-meta")));
      first = read_form(this);
      second = read_form(this);
      switch (second->type) {
      case MAL_SYMBOL:
        list_append(this, list, second);
        list_append(this, list, first);
        env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
        list_append(this, list, mal);
        return mal_list(this, list);
      default:
        return mal_error(this, ERROR_READER, text_display_position(this, token,
            "expected symbol"));
      }
    case TOKEN_BACKTICK:
      return read_symbol_list(this, "quasiquote");
    case TOKEN_LBRACE:
      return read_hashmap(this);
    case TOKEN_RBRACE:
      return mal_error(this, ERROR_READER, text_display_position(this, token,
          "unbalanced brackets, expected '{'"));
    case TOKEN_TILDE_AT:
      return read_symbol_list(this, "splice-unquote");
    case TOKEN_TILDE:
      return read_symbol_list(this, "unquote");
    default:
      return read_atom(this);
    }
  }
}

mal_p read_list(lvm_p this)
{
  return read_parenthesis(this);
}

mal_p read_parenthesis(lvm_p this)
{
  token_p beginning = reader_peek(this);
  token_p token = reader_next(this);
  list_p list = list_make(this, 0);
  mal_p mal;
  token = reader_peek(this);
  switch (token->type) {
  case TOKEN_EOI:
    return mal_error(this, ERROR_READER, text_display_position(this, beginning,
        "unbalanced parenthesis, expected ')'"));
  case TOKEN_RPAREN:
    (void)reader_next(this);
    return mal_list(this, list);
  default:
    while (TOKEN_RPAREN != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return mal_error(this, ERROR_READER, text_display_position(this,
              beginning, "unbalanced parenthesis, expected ')'"));
        } else {
          if (0 == list->count) {
            env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
            list_append(this, list, mal);
          }
          list_append(this, list, read_form(this));
          return mal_list(this, list);
        }
      }
      list_append(this, list, read_form(this));
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return mal_error(this, ERROR_READER, text_display_position(this,
            beginning, "unbalanced parenthesis, expected ')'"));
      }
    }
    token = reader_next(this);
    env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
    list_append(this, list, mal);
    return mal_list(this, list);
  }
}

mal_p read_vector(lvm_p this)
{
  return read_brackets(this);
}

mal_p read_brackets(lvm_p this)
{
  token_p beginning = reader_peek(this);
  token_p token = reader_next(this);
  vector_p vector = vector_make(this, 0);
  mal_p mal;
  switch (token->type) {
  case TOKEN_EOI:
    return mal_error(this, ERROR_READER, text_display_position(this, beginning,
        "unbalanced brackets, expected ']'"));
  case TOKEN_RBRACKET:
    (void)reader_next(this);
    return mal_vector(this, vector);
  default:
    while (TOKEN_RBRACKET != token->type) {
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
        if (TOKEN_EOI == token->type) {
          return mal_error(this, ERROR_READER, text_display_position(this,
              beginning, "unbalanced brackets, expected ']'"));
        } else {
          if (0 == vector->count) {
            env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
            vector_append(this, vector, mal);
          }
          vector_append(this, vector, read_form(this));
          return mal_vector(this, vector);
        }
      }
      vector_append(this, vector, read_form(this));
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return mal_error(this, ERROR_READER, text_display_position(this,
            beginning, "unbalanced brackets, expected ']'"));
      }
    }
    token = reader_next(this);
    env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
    vector_append(this, vector, mal);
    return mal_vector(this, vector);
  }
}

mal_p read_hashmap(lvm_p this)
{
  return read_braces(this);
}

mal_p read_braces(lvm_p this)
{
  token_p beginning = reader_peek(this);
  token_p token = reader_next(this);
  hashmap_p hashmap = hashmap_make(this, 0);
  mal_p mal = NULL;
  mal_p key = NULL;
  mal_p value = NULL;
  switch (token->type) {
  case TOKEN_EOI:
    return mal_error(this, ERROR_READER, text_display_position(this, beginning,
        "unbalanced braces, expected '}'"));
  case TOKEN_RBRACE:
    (void)reader_next(this);
    value = mal_hashmap(this, hashmap);
    return value;
  default:
    while (TOKEN_RBRACE != token->type) {
      value = NULL;
      if (TOKEN_COLON == token->type) { /* TODO: to be removed in the future */
          return mal_error(this, ERROR_READER, text_display_position(this,
              beginning, "unexpected colon ':'"));
      }
      key = read_form(this);
      token = reader_peek(this);
      if (TOKEN_COLON == token->type) {
        token = reader_next(this);
      }
      switch (token->type) {
      case TOKEN_EOI:
        return mal_error(this, ERROR_READER, text_display_position(this,
            beginning, "unbalanced braces, expected '}'"));
      case TOKEN_RBRACE:
        env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
        value = mal;
        break;
      default:
        value = read_form(this);
        break;
      }
      hashmap_set(this, hashmap, key, value);
      token = reader_peek(this);
      if (TOKEN_EOI == token->type) {
        return mal_error(this, ERROR_READER, text_display_position(this,
            beginning, "unbalanced braces, expected '}'"));
      }
    }
    token = reader_next(this);
    return mal_hashmap(this, hashmap);
  }
}

mal_p read_atom(lvm_p this)
{
  token_p token = reader_peek(this);
  mal_p mal;
  reader_next(this);
  switch (token->type) {
  case TOKEN_EOI:
    return mal_eoi(this);
  case TOKEN_NIL:
    return mal_nil(this);
  case TOKEN_BOOLEAN:
    if (0 == text_cmp(this, token->as.boolean, "true")) {
      env_get_by_text(this, this->env, text_make(this, "boolean: true"), &mal);
      return mal;
    } else {
      env_get_by_text(this, this->env, text_make(this, "boolean: false"), &mal);
      return mal;
    }
  case TOKEN_SYMBOL:
    if (0 == text_cmp(this, token->as.symbol, "nil")) {
      env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
      return mal;
    } else if (0 == text_cmp(this, token->as.symbol, "true")) {
      env_get_by_text(this, this->env, text_make(this, "boolean: true"), &mal);
      return mal;
    } else if (0 == text_cmp(this, token->as.symbol, "false")) {
      env_get_by_text(this, this->env, text_make(this, "boolean: false"), &mal);
      return mal;
    } else {
      return mal_symbol(this, token->as.symbol);
    }
  case TOKEN_KEYWORD:
    return mal_keyword(this, token->as.keyword);
  case TOKEN_STRING:
    return mal_string(this, token->as.string);
  case TOKEN_INTEGER:
    return mal_integer(this, text_to_integer(this, token->as.number));
  case TOKEN_DECIMAL:
    return mal_decimal(this, text_to_decimal(this, token->as.number));
  default:
    return mal_error(this, ERROR_READER, text_display_position(this, token,
        "unknown atom type"));
  }
}

mal_p read_symbol_list(lvm_p this, char *name)
{
  list_p list = list_make(this, 0);
  mal_p mal;
  reader_next(this);
  list_append(this, list, mal_symbol(this, text_make(this, name)));
  list_append(this, list, read_form(this));
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &mal);
  list_append(this, list, mal);
  return mal_list(this, list);
}

mal_p mal_make(lvm_p this, mal_type type)
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

mal_p mal_eoi(lvm_p this)
{
  mal_p mal = mal_make(this, MAL_EOI);
  text_p eoi = text_make(this, "eoi");
  text_p identity = eoi;
  text_p signature = text_make(this, "eoi: eoi");
  mal->as.eoi = eoi;
  mal->token->as.eoi = eoi;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_nil(lvm_p this)
{
  mal_p mal = mal_make(this, MAL_NIL);
  text_p nil = text_make(this, "nil");
  text_p identity = nil;
  text_p signature = text_make(this, "nil: nil");
  mal->as.nil = nil;
  mal->token->as.nil = nil;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_error(lvm_p this, error_type type, text_p text)
{
  mal_p mal = mal_make(this, MAL_ERROR);
  text_p error;
  text_p identity;
  text_p signature = text_concat_text(this, text_make(this, "error: "), text);
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
  identity = error;
  mal->as.error = error;
  mal->token->as.error = error;
  error_append(this, type, text);
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_boolean(lvm_p this, bool boolean)
{
  mal_p mal = mal_make(this, MAL_BOOLEAN);
  text_p text;
  text_p identity;
  text_p signature = text_make(this, "boolean: ");
  if (boolean) {
    text = text_make(this, "true");
  } else {
    text = text_make(this, "false");
  }
  identity = text;
  mal->as.boolean = boolean;
  mal->token->as.boolean = text;
  mal->signature = text_concat_text(this, signature, text);
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_symbol(lvm_p this, text_p symbol)
{
  mal_p mal = mal_make(this, MAL_SYMBOL);
  text_p identity = symbol;
  text_p signature = text_concat_text(this, text_make(this, "symbol: "),
      symbol);
  mal->as.symbol = symbol;
  mal->token->as.symbol = symbol;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_keyword(lvm_p this, text_p keyword)
{
  mal_p mal = mal_make(this, MAL_KEYWORD);
  text_p identity = keyword;
  text_p signature = text_concat_text(this, text_make(this, "keyword: "),
      keyword);
  mal->as.keyword = keyword;
  mal->token->as.keyword = keyword;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_string(lvm_p this, text_p string)
{
  mal_p mal = mal_make(this, MAL_STRING);
  text_p identity = string;
  text_p signature = text_concat_text(this, text_make(this, "string: "),
      string);
  mal->as.string = string;
  mal->token->as.string = string;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_function(lvm_p this, function_p function)
{
  mal_p mal = mal_make(this, MAL_FUNCTION);
  text_p identity = function->name;
  text_p signature = text_concat_text(this, text_make(this, "function: "),
      function->name);
  mal->as.function = function;
  mal->token->as.function = function->name;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_closure(lvm_p this, closure_p closure)
{
  mal_p mal = mal_make(this, MAL_CLOSURE);
  text_p identity = closure_text(this, closure);
  text_p signature = text_concat_text(this, text_make(this, "closure: "),
      identity);
  mal->as.closure = closure;
  mal->token->as.closure = identity;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_list(lvm_p this, list_p list)
{
  mal_p mal = mal_make(this, MAL_LIST);
  text_p identity = list_text(this, list);
  text_p signature = text_concat_text(this, text_make(this, "list: "),
      list_text(this, list));
  mal->as.list = list;
  mal->token->as.list = identity;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_vector(lvm_p this, vector_p vector)
{
  mal_p mal = mal_make(this, MAL_VECTOR);
  text_p identity = vector_text(this, vector);
  text_p signature = text_concat_text(this, text_make(this, "vector: "),
      vector_text(this, vector));
  mal->as.vector = vector;
  mal->token->as.vector = identity;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_hashmap(lvm_p this, hashmap_p hashmap)
{
  mal_p mal = mal_make(this, MAL_HASHMAP);
  text_p identity = hashmap_text(this, hashmap);
  text_p signature = text_concat_text(this, text_make(this, "hashmap: "),
      hashmap_text(this, hashmap));
  mal->as.hashmap = hashmap;
  mal->token->as.hashmap = identity;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_env(lvm_p this, env_p env)
{
  mal_p mal = mal_make(this, MAL_ENV);
  text_p identity = env_text(this, env);
  text_p signature = text_concat_text(this, text_make(this, "env: "),
      env_text(this, env));
  mal->as.env = env;
  mal->token->as.env = identity;
  mal->signature = signature;
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_integer(lvm_p this, long integer)
{
  mal_p mal = mal_make(this, MAL_INTEGER);
  text_p identity = text_make_integer(this, integer);
  text_p signature = text_make(this, "integer: ");
  mal->as.integer = integer;
  mal->token->as.number = identity;
  mal->signature = text_concat_text(this, signature, identity);
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_decimal(lvm_p this, double decimal)
{
  mal_p mal = mal_make(this, MAL_DECIMAL);
  text_p identity = text_make_decimal(this, decimal);
  text_p signature = text_make(this, "decimal: ");
  mal->as.decimal = decimal;
  mal->token->as.number = identity;
  mal->signature = text_concat_text(this, signature, identity);
  mal->identity = identity;
  mal->hash = text_hash_jenkins(this, signature);
  return mal;
}

mal_p mal_as_str(lvm_p this, mal_p args, bool readable, char *separator)
{
  text_p text = text_make(this, "");
  size_t at;
  for (at = 0; at < args->as.list->count - 2; at++) {
    text_concat_text(this, text, mal_print(this, args->as.list->data[at],
        readable));
    text_concat(this, text, separator);
  }
  text_concat_text(this, text, mal_print(this, args->as.list->data[at],
      readable));
  return mal_string(this, text);
}

text_p mal_print(lvm_p this, mal_p mal, bool readable)
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
  case MAL_FUNCTION:
    return mal->as.function->name;
  case MAL_CLOSURE:
    return closure_text(this, mal->as.closure);
  case MAL_LIST:
    text = text_make(this, "(");
    if (mal->as.list->count > 0) {
      if ((mal->as.list->count == 1 && !is_nil(mal->as.list->data[0])) ||
          !is_nil(mal->as.list->data[0])) {
        text_concat_text(this, text, mal_print(this, mal->as.list->data[0],
            readable));
      }
      for (; i < (mal->as.list->count - 1); i++) {
        text_append(this, text, ' ');
        text_concat_text(this, text, mal_print(this, mal->as.list->data[i],
            readable));
      }
      if (i != 1 && !is_nil(mal->as.list->data[i])) {
        text_concat(this, text, " : ");
        text_concat_text(this, text, mal_print(this,
            mal->as.list->data[i], readable));
      }
    }
    text_append(this, text, ')');
    return text_append(this, text, 0x00);
  case MAL_VECTOR:
    text = text_make(this, "[");
    if (mal->as.vector->count > 0) {
      if ((mal->as.vector->count == 1 && !is_nil(mal->as.vector->data[0])) ||
          !is_nil(mal->as.vector->data[0])) {
        text_concat_text(this, text, mal_print(this, mal->as.vector->data[0],
            readable));
      }
      for (; i < (mal->as.vector->count - 1); i++) {
        text_append(this, text, ' ');
        text_concat_text(this, text, mal_print(this, mal->as.vector->data[i],
            readable));
      }
      if (i != 1 && !is_nil(mal->as.vector->data[i])) {
        text_concat(this, text, " : ");
        text_concat_text(this, text, mal_print(this,
            mal->as.vector->data[i], readable));
      }
    }
    text_append(this, text, ']');
    return text_append(this, text, 0x00);
  case MAL_HASHMAP:
    text = text_make(this, "{");
    if (mal->as.hashmap->count > 0) {
      text_concat_text(this, text, mal_print(this, mal->as.hashmap->data[0],
          readable));
      text_concat(this, text, ": ");
      text_concat_text(this, text, mal_print(this, mal->as.hashmap->data[1],
          readable));
      for (i = 2; i < (mal->as.hashmap->count); i += 2) {
        text_append(this, text, ' ');
        text_concat_text(this, text, mal_print(this, mal->as.hashmap->data[i],
            readable));
        text_concat(this, text, ": ");
        text_concat_text(this, text, mal_print(this,
            mal->as.hashmap->data[i + 1], readable));
      }
    }
    text_append(this, text, '}');
    return text_append(this, text, 0x00);
  case MAL_ENV:
    text = text_make(this, "{");
    if (mal->as.env->hashmap->count > 0) {
      text_concat_text(this, text, mal_print(this,
          mal->as.env->hashmap->data[0], readable));
      text_concat(this, text, ": ");
      text_concat_text(this, text, mal_print(this,
          mal->as.env->hashmap->data[1], readable));
      for (i = 2; i < (mal->as.env->hashmap->count); i += 2) {
        text_append(this, text, ' ');
        text_concat_text(this, text, mal_print(this,
            mal->as.env->hashmap->data[i], readable));
        text_concat(this, text, ": ");
        text_concat_text(this, text, mal_print(this,
            mal->as.env->hashmap->data[i + 1], readable));
      }
    }
    text_append(this, text, '}');
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

void mal_free(lvm_p this, gc_p mal)
{
  (void)this;
  free((void *)((mal_p)mal));
}

bool is_eoi(mal_p mal)
{
  return (MAL_EOI == mal->type);
}

bool is_nil(mal_p mal)
{
  return (MAL_NIL == mal->type);
}

bool is_true(mal_p mal)
{
  return (MAL_BOOLEAN == mal->type && true == mal->as.boolean);
}

bool is_false(mal_p mal)
{
  return (MAL_BOOLEAN == mal->type && false == mal->as.boolean);
}

bool is_error(mal_p mal)
{
  return (MAL_ERROR == mal->type);
}

bool is_integer(mal_p mal)
{
  return (MAL_INTEGER == mal->type);
}

bool is_decimal(mal_p mal)
{
  return (MAL_DECIMAL == mal->type);
}

bool is_number(mal_p mal)
{
  return (MAL_INTEGER == mal->type || MAL_DECIMAL == mal->type);
}

bool is_symbol(mal_p mal)
{
  return (MAL_SYMBOL == mal->type);
}

bool is_keyword(mal_p mal)
{
  return (MAL_KEYWORD == mal->type);
}

bool is_string(mal_p mal)
{
  return (MAL_STRING == mal->type);
}

bool is_self_evaluating(mal_p mal)
{
  return (MAL_NIL == mal->type || MAL_BOOLEAN == mal->type ||
      MAL_KEYWORD == mal->type || MAL_STRING == mal->type ||
      MAL_INTEGER == mal->type || MAL_DECIMAL == mal->type);
}

bool is_list(mal_p mal)
{
  return (MAL_LIST == mal->type);
}

bool is_vector(mal_p mal)
{
  return (MAL_VECTOR == mal->type);
}

bool is_hashmap(mal_p mal)
{
  return (MAL_HASHMAP == mal->type);
}

bool is_env(mal_p mal)
{
  return (MAL_ENV == mal->type);
}

bool is_sequential(mal_p mal)
{
  return (MAL_LIST == mal->type || MAL_VECTOR == mal->type);
}

bool is_function(mal_p mal)
{
  return (MAL_FUNCTION == mal->type);
}

bool is_closure(mal_p mal)
{
  return (MAL_CLOSURE == mal->type);
}

bool is_callable(mal_p mal)
{
  return (MAL_FUNCTION == mal->type || MAL_CLOSURE);
}

lvm_p lvm_make()
{
  lvm_p lvm = (lvm_p)calloc(1, sizeof(lvm_t));
  mal_p mal;
  lvm->gc.mark = 0;
  lvm->gc.count = 0;
  lvm->gc.total = 8;
  lvm->gc.first = NULL;
  lvm->error = NULL;
  lvm->comment = NULL;
  lvm->env = env_make(lvm, NULL, NULL, NULL, NULL, 0);
  mal = mal_nil(lvm);
  env_set(lvm, lvm->env, mal, mal);
  mal = mal_boolean(lvm, true);
  env_set(lvm, lvm->env, mal, mal);
  mal = mal_boolean(lvm, false);
  env_set(lvm, lvm->env, mal, mal);
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
  case GC_FUNCTION:
    ((function_p)gc)->gc.mark = this->gc.mark;
    lvm_gc_mark(this, (gc_p)(((function_p)gc)->name));
    break;
  case GC_CLOSURE:
    ((closure_p)gc)->gc.mark = this->gc.mark;
    lvm_gc_mark(this, (gc_p)(((closure_p)gc)->parameters));
    lvm_gc_mark(this, (gc_p)(((closure_p)gc)->more));
    lvm_gc_mark(this, (gc_p)(((closure_p)gc)->definition));
    break;
  case GC_LIST:
    gc->mark = this->gc.mark;
    for (at = 0; at < ((list_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((list_p)gc)->data[at]));
    }
    break;
  case GC_VECTOR:
    gc->mark = this->gc.mark;
    for (at = 0; at < ((vector_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((vector_p)gc)->data[at]));
    }
    break;
  case GC_HASHMAP:
    ((hashmap_p)gc)->gc.mark = this->gc.mark;
    for (at = 0; at < ((hashmap_p)gc)->count; at++) {
      lvm_gc_mark(this, (gc_p)(((hashmap_p)gc)->data[at]));
    }
    break;
  case GC_ENV:
    ((env_p)gc)->hashmap->gc.mark = this->gc.mark;
    for (at = 0; at < ((env_p)gc)->hashmap->count; at++) {
      lvm_gc_mark(this, (gc_p)(((env_p)gc)->hashmap->data[at]));
    }
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
  case GC_TOKEN:
    break;
  case GC_MAL:
    ((mal_p)gc)->gc.mark = this->gc.mark;
    switch (((mal_p)gc)->type) {
    case MAL_FUNCTION:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.function);
      break;
    case MAL_CLOSURE:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.closure);
      break;
    case MAL_LIST:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.list);
      break;
    case MAL_VECTOR:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.vector);
      break;
    case MAL_HASHMAP:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.hashmap);
      break;
    case MAL_ENV:
      lvm_gc_mark(this, (gc_p)((mal_p)gc)->as.env);
      break;
    case MAL_ERROR:
    default:
      break;
    }
    lvm_gc_mark(this, (gc_p)((mal_p)gc)->signature);
    lvm_gc_mark(this, (gc_p)((mal_p)gc)->identity);
    lvm_gc_mark(this, (gc_p)((mal_p)gc)->token);
    break;
  }
}

void lvm_gc_mark_all(lvm_p this)
{
  (void)this;
  lvm_gc_mark(this, (gc_p)this->env);
}

void lvm_gc_sweep(lvm_p this)
{
  gc_pp obj = &this->gc.first;
  while (*obj) {
    if (this->gc.mark != (*obj)->mark) {
      gc_p unreached = (*obj);
      (*obj) = unreached->next;
      switch (unreached->type) {
      case GC_TEXT:
        text_free(this, unreached);
        break;
      case GC_FUNCTION:
        function_free(this, unreached);
        break;
      case GC_CLOSURE:
        closure_free(this, unreached);
        break;
      case GC_LIST:
        list_free(this, unreached);
        break;
      case GC_VECTOR:
        vector_free(this, unreached);
        break;
      case GC_HASHMAP:
        hashmap_free(this, unreached);
        break;
      case GC_ENV:
        env_free(this, unreached);
        break;
      case GC_ERROR:
        error_free(this, unreached);
        break;
      case GC_COMMENT:
        comment_free(this, unreached);
        break;
      case GC_TOKEN:
        token_free(this, unreached);
        break;
      case GC_MAL:
        mal_free(this, unreached);
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
  size_t at;
  while (gc) {
    switch (gc->type) {
    case GC_TEXT:
      printf("text: %s\n", ((text_p)gc)->data);
      break;
    case GC_FUNCTION:
      printf("function: %s\n", ((function_p)gc)->name->data);
      break;
    case GC_CLOSURE:
      printf("closure: %s\n", closure_text(this, ((closure_p)gc)));
      break;
    case GC_LIST:
      printf("list: (");
      if (((list_p)gc)->count) {
        printf("%s", ((list_p)gc)->data[0]->identity->data);
        for (at = 1; at < ((list_p)gc)->count - 1; at++) {
          printf(" %s", ((list_p)gc)->data[at]->identity->data);
        }
        if (!is_nil(((list_p)gc)->data[at])) {
          printf(" : %s", ((list_p)gc)->data[at]->identity->data);
        }
      }
      printf(")\n");
      break;
    case GC_VECTOR:
      printf("vector: [");
      if (((vector_p)gc)->count) {
        printf("%s", ((vector_p)gc)->data[0]->identity->data);
        for (at = 1; at < ((vector_p)gc)->count - 1; at++) {
          printf(" %s", ((vector_p)gc)->data[at]->identity->data);
        }
        if (!is_nil(((vector_p)gc)->data[at])) {
          printf(" : %s", ((vector_p)gc)->data[at]->identity->data);
        }
      }
      printf("]\n");
      break;
    case GC_HASHMAP:
      printf("hashmap: {");
      if (((hashmap_p)gc)->count) {
        printf("%s: %s", ((hashmap_p)gc)->data[0]->identity->data,
            ((hashmap_p)gc)->data[1]->identity->data);
        for (at = 2; at < ((hashmap_p)gc)->count; at += 2) {
          printf(" %s: %s", ((hashmap_p)gc)->data[at]->identity->data,
              ((hashmap_p)gc)->data[at + 1]->identity->data);
        }
      }
      printf("}\n");
      break;
    case GC_ENV:
      printf("env: {");
      if (((env_p)gc)->hashmap->count) {
        printf("%s: %s", ((env_p)gc)->hashmap->data[0]->identity->data,
            ((env_p)gc)->hashmap->data[1]->identity->data);
        for (at = 2; at < ((env_p)gc)->hashmap->count; at += 2) {
          printf(" %s: %s", ((env_p)gc)->hashmap->data[at]->identity->data,
              ((env_p)gc)->hashmap->data[at + 1]->identity->data);
        }
      }
      printf("}\n");
      break;
    case GC_ERROR:
      printf("error: %s\n", error_collapse(this)->data);
      break;
    case GC_COMMENT:
      printf("comment: %s\n", comment_collapse(this)->data);
      break;
    case GC_TOKEN:
      printf("token: %s\n", ((token_p)gc)->as.symbol->data);
      break;
    case GC_MAL:
      printf("mal: %s\n", mal_print(this, ((mal_p)gc), false)->data);
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
    case GC_FUNCTION:
      function_free(this, tmp);
      break;
    case GC_CLOSURE:
      closure_free(this, tmp);
      break;
    case GC_LIST:
      list_free(this, tmp);
      break;
    case GC_VECTOR:
      vector_free(this, tmp);
      break;
    case GC_HASHMAP:
      hashmap_free(this, tmp);
      break;
    case GC_ENV:
      vector_free(this, tmp);
      break;
    case GC_ERROR:
      error_free(this, tmp);
      break;
    case GC_COMMENT:
      comment_free(this, tmp);
      break;
    case GC_TOKEN:
      token_free(this, tmp);
      break;
    case GC_MAL:
      mal_free(this, tmp);
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

mal_p eval_ast(lvm_p this, mal_p ast, env_p env)
{
  mal_p result;
  switch (ast->type) {
  case MAL_SYMBOL:
    if (env_get(this, env, ast, &result)){
      return result;
    } else {
#if VAR_NIL
      env_get_by_text(this, env, text_make(this, "nil: nil"), &result);
      return result;
#else
      return mal_error(this, ERROR_RUNTIME,
          text_concat(this, text_concat_text(this, text_make(this,
          "var '"), mal_print(this, ast, false)), "' not found\n"));
#endif
    }
    break;
  case MAL_LIST:
    result = eval_list(this, ast->as.list, env);
    if (is_error(result)) {
      return result;
    } else {
      return result;
    }
    break;
  case MAL_VECTOR:
    result = eval_vector(this, ast->as.vector, env);
    if (is_error(result)) {
      return result;
    } else {
      return result;
    }
    break;
  case MAL_HASHMAP:
    result = eval_hashmap(this, ast->as.hashmap, env);
    if (is_error(result)) {
      return result;
    } else {
      return result;
    }
    break;
  default:
    return ast;
  }
}

mal_p eval_list(lvm_p this, list_p original, env_p env)
{
  list_p evaluated = list_make(this, original->count);
  size_t at;
  for (at = 0; at < original->count; at++) {
    list_append(this, evaluated, lvm_eval(this, original->data[at], env));
  }
  return mal_list(this, evaluated);
}

mal_p eval_vector(lvm_p this, vector_p original, env_p env)
{
  vector_p evaluated = vector_make(this, original->count);
  size_t at;
  for (at = 0; at < original->count; at++) {
    vector_append(this, evaluated, lvm_eval(this, original->data[at], env));
  }
  return mal_vector(this, evaluated);
}

mal_p eval_hashmap(lvm_p this, hashmap_p original, env_p env)
{
  size_t at;
  hashmap_p evaluated = hashmap_make(this, original->count >> 1);
  for (at = 0; at < original->count; at += 2) {
    hashmap_set(this, evaluated, original->data[at],
        lvm_eval(this, original->data[at + 1], env));
  }
  return mal_hashmap(this, evaluated);
}

mal_p eval_def_bang(lvm_p this, mal_p ast, env_p env)
{
  list_p list = list_params(this, ast->as.list);
  mal_p symbol;
  mal_p value;
  mal_p result;
  if (2 > list->count || (2 < list->count && !is_nil(list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'def!': expected proper list with two arguments\n"));
  }
  symbol = list->data[0];
  if (!is_symbol(symbol)) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'def!': expected symbol as first argument\n"));
  }
  value = list_params(this, list)->data[0];
  result = lvm_eval(this, value, env);

  if (!is_error(result)) {
    env_set(this, env, symbol, result);
  }
  return result;
}

mal_p eval_let_star(lvm_p this, mal_p ast, env_pp outer)
{
  list_p bindings;
  env_p env;
  size_t at = 0;
  mal_p forms;
  mal_p nil;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  if (2 > ast->as.list->count) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'let*': missing binding list\n"));
  }
  forms = 2 <= ast->as.list->count ? ast->as.list->data[2] : nil;
  if (!is_sequential(ast->as.list->data[1])) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'let*': first argument is not list or vector\n"));
  }
  bindings = ast->as.list->data[1]->as.list;
  if ((bindings->count % 2 == 0 && is_nil(bindings->data[bindings->count - 1])) || (
      bindings->count % 2 == 1 && !is_nil(bindings->data[bindings->count - 1]))) {
#if VAR_NIL
    list_append(this, bindings, nil);
#else
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'let*': expected an even number of binding pairs\n"));
#endif
  }
  env = env_make(this, (*outer), NULL, NULL, NULL, 0);
  if ((4 == ast->as.list->count && is_nil(ast->as.list->data[3])) ||
      3 == ast->as.list->count) {
    for (at = 0; at < bindings->count - 1; at += 2) {
      mal_p symbol = bindings->data[at];
      mal_p value = lvm_eval(this, bindings->data[at + 1], env);
      if (is_error(value)) {
        return value;
      }
      if (!is_symbol(symbol)) {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "'let*': binding error, '"), mal_print(this, symbol, false)),
            "' is not a symbol\n"));
      }
      env_set(this, env, symbol, value);
    }
    (*outer) = env;
    return forms;
  }
  return mal_error(this, ERROR_RUNTIME, text_make(this,
      "'let*': expected the proper list with exactly three elements\n"));
}

mal_p eval_if(lvm_p this, mal_p ast, env_pp env)
{
  list_p list = ast->as.list;
  mal_p condition;
  if (list->count < 3 || (3 == list->count && is_nil(list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_concat(this,
        text_concat_text(this, text_make(this, "'if': got too few arguments '"),
        text_make_integer(this, list->count - 2)), "'\n"));
  }
  if (list->count > 4 && !is_nil(list->data[4])) {
    return mal_error(this, ERROR_RUNTIME, text_concat(this,
        text_concat_text(this, text_make(this, "'if': got too many arguments '"),
        text_make_integer(this, list->count - 2)), "'\n"));
  }
  condition = lvm_eval(this, list->data[1], *env);
  if (is_error(condition)) {
    return condition;
  }
  if (is_false(condition) || is_nil(condition)) {
    if (!is_nil(list->data[3])) {
      return list->data[3];
    } else {
      mal_p nil;
      env_get_by_text(this, *env, text_make(this, "nil: nil"), &nil);
      return nil;
    }
  } else {
    return list->data[2];
  }
}

mal_p eval_fn_star(lvm_p this, mal_p ast, env_p env)
{
  mal_p result;
  mal_p params;
  mal_p more = NULL;
  mal_p definition;
  list_p list = ast->as.list;
  if (3 > list->count && is_nil(list->data[1])) {
    return mal_error(this, ERROR_RUNTIME, text_concat(this,
        text_concat_text(this, text_make(this, "'fn*': has too few arguments '"),
        text_make_integer(this, list->count - 2)),
        "' missing parameters and body\n"));
  }
  if (4 > list->count && is_nil(list->data[2])) {
    return mal_error(this, ERROR_RUNTIME, text_concat(this,
        text_concat_text(this, text_make(this, "'fn*': has too few arguments '"),
        text_make_integer(this, list->count - 2)), "' missing body\n"));
  }
  if (4 <= list->count && !is_nil(list->data[3])) {
    return mal_error(this, ERROR_RUNTIME, text_concat(this,
        text_concat_text(this, text_make(this,
        "'fn*': has too many arguments '"),
        text_make_integer(this, list->count - 2)), "'\n"));
  }
  params = list->data[1];
  result = closure_parameters(this, &params, &more);
  if (is_error(result)) {
    return result;
  }
  definition = list->data[2];
  return mal_closure(this, closure_make(this, env, params, definition, more));
}

mal_p eval_do(lvm_p this, mal_p ast, env_p env)
{
  list_p list = list_params(this, ast->as.list);
  mal_p mal;
  size_t at;
  if (0 == list->count || (1 == list->count && is_nil(list->data[0]))) {
    mal_p nil;
    env_get_by_text(this, env, text_make(this, "nil: nil"), &nil);
    return nil;
  }
  for (at = 0; at < list->count - 1; at++) {
    mal = lvm_eval(this, list->data[at], env);
    if (is_error(mal)) {
      return mal;
    }
  }
  if (is_nil(list->data[at])) {
    return mal;
  } else {
    return list->data[at];
  }
}

mal_p core_add(lvm_p this, mal_p args)
{
  mal_type type = MAL_INTEGER;
  list_p arg_list = args->as.list;
  union {
    long integer;
    double decimal;
  } sum;
  mal_p result = mal_integer(this, 0);
  if (0 == arg_list->count || (is_nil(arg_list->data[0]) &&
      1 == arg_list->count)) {
    return result;
  }
  switch (arg_list->data[0]->type) {
  case MAL_INTEGER:
    result->as.integer = arg_list->data[0]->as.integer;
    break;
  case MAL_DECIMAL:
    result->type = type = MAL_DECIMAL;
    result->as.decimal = arg_list->data[0]->as.decimal;
    break;
  default:
    return mal_error(this, ERROR_RUNTIME,
        text_concat(this, text_concat_text(this, text_make(this,
        "args to '+' are not numbers '"), mal_print(this,
        args->as.list->data[0], false)), "'\n"));
  }

  if (1 < arg_list->count) {
    size_t at;

    for (at = 1; at < arg_list->count; at++) {
      if (at == arg_list->count - 1 && is_nil(arg_list->data[at])) {
        break;
      }
      if (is_integer(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          sum.integer = result->as.integer + arg_list->data[at]->as.integer;
          result->as.integer = sum.integer;
        } else if (MAL_DECIMAL == type) {
          sum.decimal = result->as.decimal +
              (double)arg_list->data[at]->as.integer;
          result->type = MAL_DECIMAL;
          result->as.decimal = sum.decimal;
        }
      } else if (is_decimal(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          sum.decimal = (double)result->as.integer +
              arg_list->data[at]->as.decimal;
          result->type = type = MAL_DECIMAL;
        } else if (MAL_DECIMAL == type) {
          sum.decimal = result->as.decimal + arg_list->data[at]->as.decimal;
        }
        result->as.decimal = sum.decimal;
      } else {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "args to '+' are not numbers '"), mal_print(this,
            args->as.list->data[at], false)), "'\n"));
      }
    }
  }
  switch (type) {
  case MAL_INTEGER:
  case MAL_DECIMAL:
    return result;
  default:
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "unknown type of '+' result\n"));
  }
}

mal_p core_sub(lvm_p this, mal_p args)
{
  mal_type type = MAL_INTEGER;
  list_p arg_list = args->as.list;
  union {
    long integer;
    double decimal;
  } difference;
  mal_p result = mal_integer(this, 0);
  if (0 == arg_list->count || (is_nil(arg_list->data[0]) &&
      1 == arg_list->count)) {
    return result;
  }
  switch (arg_list->data[0]->type) {
  case MAL_INTEGER:
    result->as.integer = arg_list->data[0]->as.integer;
    break;
  case MAL_DECIMAL:
    result->type = type = MAL_DECIMAL;
    result->as.decimal = arg_list->data[0]->as.decimal;
    break;
  default:
    return mal_error(this, ERROR_RUNTIME,
        text_concat(this, text_concat_text(this, text_make(this,
        "args to '-' are not numbers '"), mal_print(this,
        args->as.list->data[0], false)), "'\n"));
  }

  if (1 < arg_list->count) {
    size_t at;

    for (at = 1; at < arg_list->count; at++) {
      if (at == arg_list->count - 1 && is_nil(arg_list->data[at])) {
        break;
      }
      if (is_integer(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          difference.integer = result->as.integer -
              arg_list->data[at]->as.integer;
          result->as.integer = difference.integer;
        } else if (MAL_DECIMAL == type) {
          difference.decimal = result->as.decimal -
              (double)arg_list->data[at]->as.integer;
          result->type = MAL_DECIMAL;
          result->as.decimal = difference.decimal;
        }
      } else if (is_decimal(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          difference.decimal = (double)result->as.integer -
              arg_list->data[at]->as.decimal;
          result->type = type = MAL_DECIMAL;
        } else if (MAL_DECIMAL == type) {
          difference.decimal = result->as.decimal -
              arg_list->data[at]->as.decimal;
        }
        result->as.decimal = difference.decimal;
      } else {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "args to '-' are not numbers '"), mal_print(this,
            args->as.list->data[at], false)), "'\n"));
      }
    }
  }
  switch (type) {
  case MAL_INTEGER:
  case MAL_DECIMAL:
    return result;
  default:
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "unknown type of '-' result\n"));
  }
}

mal_p core_mul(lvm_p this, mal_p args)
{
  mal_type type = MAL_INTEGER;
  list_p arg_list = args->as.list;
  union {
    long integer;
    double decimal;
  } product;
  mal_p result = mal_integer(this, 1);
  if (0 == arg_list->count || (is_nil(arg_list->data[0]) &&
      1 == arg_list->count)) {
    return result;
  }
  switch (arg_list->data[0]->type) {
  case MAL_INTEGER:
    result->as.integer = arg_list->data[0]->as.integer;
    break;
  case MAL_DECIMAL:
    result->type = type = MAL_DECIMAL;
    result->as.decimal = arg_list->data[0]->as.decimal;
    break;
  default:
    return mal_error(this, ERROR_RUNTIME,
        text_concat(this, text_concat_text(this, text_make(this,
        "args to '*' are not numbers '"), mal_print(this,
        args->as.list->data[0], false)), "'\n"));
  }

  if (1 < arg_list->count) {
    size_t at;

    for (at = 1; at < arg_list->count; at++) {
      if (at == arg_list->count - 1 && is_nil(arg_list->data[at])) {
        break;
      }
      if (is_integer(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          product.integer = result->as.integer * arg_list->data[at]->as.integer;
          result->as.integer = product.integer;
        } else if (MAL_DECIMAL == type) {
          product.decimal = result->as.decimal *
              (double)arg_list->data[at]->as.integer;
          result->type = MAL_DECIMAL;
          result->as.decimal = product.decimal;
        }
      } else if (is_decimal(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          product.decimal = (double)result->as.integer *
              arg_list->data[at]->as.decimal;
          result->type = type = MAL_DECIMAL;
        } else if (MAL_DECIMAL == type) {
          product.decimal = result->as.decimal * arg_list->data[at]->as.decimal;
        }
        result->as.decimal = product.decimal;
      } else {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "args to '*' are not numbers '"), mal_print(this,
            args->as.list->data[at], false)), "'\n"));
      }
    }
  }
  switch (type) {
  case MAL_INTEGER:
  case MAL_DECIMAL:
    return result;
  default:
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "unknown type of '*' result\n"));
  }
}

mal_p core_div(lvm_p this, mal_p args)
{
  mal_type type = MAL_INTEGER;
  list_p arg_list = args->as.list;
  union {
    long integer;
    double decimal;
  } quotient;
  mal_p result = mal_integer(this, 1);
  if (0 == arg_list->count || (is_nil(arg_list->data[0]) &&
      1 == arg_list->count)) {
    return result;
  }
  switch (arg_list->data[0]->type) {
  case MAL_INTEGER:
    result->as.integer = arg_list->data[0]->as.integer;
    break;
  case MAL_DECIMAL:
    result->type = type = MAL_DECIMAL;
    result->as.decimal = arg_list->data[0]->as.decimal;
    break;
  default:
    return mal_error(this, ERROR_RUNTIME,
        text_concat(this, text_concat_text(this, text_make(this,
        "args to '/' are not numbers '"), mal_print(this,
        args->as.list->data[0], false)), "'\n"));
  }

  if (1 < arg_list->count) {
    size_t at;

    for (at = 1; at < arg_list->count; at++) {
      if (at == arg_list->count - 1 && is_nil(arg_list->data[at])) {
        break;
      }
      if (is_integer(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          quotient.integer = result->as.integer /
              arg_list->data[at]->as.integer;
          result->as.integer = quotient.integer;
        } else if (MAL_DECIMAL == type) {
          quotient.decimal = result->as.decimal /
              (double)arg_list->data[at]->as.integer;
          result->type = MAL_DECIMAL;
          result->as.decimal = quotient.decimal;
        }
      } else if (is_decimal(arg_list->data[at])) {
        if (MAL_INTEGER == type) {
          quotient.decimal = (double)result->as.integer /
              arg_list->data[at]->as.decimal;
          result->type = type = MAL_DECIMAL;
        } else if (MAL_DECIMAL == type) {
          quotient.decimal = result->as.decimal /
              arg_list->data[at]->as.decimal;
        }
        result->as.decimal = quotient.decimal;
      } else {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "args to '/' are not numbers '"), mal_print(this,
            args->as.list->data[at], false)), "'\n"));
      }
    }
  }
  switch (type) {
  case MAL_INTEGER:
  case MAL_DECIMAL:
    return result;
  default:
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "unknown type of '/' result\n"));
  }
}

mal_p core_eq(lvm_p this, mal_p args)
{
  mal_p first;
  mal_p second;
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);      
  if (2 > args->as.list->count || (2 < args->as.list->count &&
      !is_nil(args->as.list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'=': expected exactly two arguments\n"));
  }
  first = args->as.list->data[0];
  second = args->as.list->data[1];
  if (first->type != second->type) {
    return f;
  } else if (is_sequential(first) && is_sequential(second)) {
    switch (first->type) {
    case MAL_LIST:
      return list_equal(this, first->as.list, second->as.list); 
    case MAL_VECTOR:
      return vector_equal(this, first->as.vector, second->as.vector);
    default:
      return mal_error(this, ERROR_RUNTIME, text_make(this,
          "'=': this state should not happen\n"));
    }
  } else {
    switch (first->type) {
    case MAL_INTEGER:
      if (first->as.integer == second->as.integer) {
        return t;
      } else {
        return f;
      }
    case MAL_DECIMAL:
      if (first->as.decimal == second->as.decimal) {
        return t;
      } else {
        return f;
      }
    case MAL_SYMBOL:
      if (strcmp(first->as.symbol->data, second->as.symbol->data) == 0) {
        return t;
      } else {
        return f;
      }
    case MAL_KEYWORD:
      if (strcmp(first->as.keyword->data, second->as.keyword->data) == 0) {
        return t;
      } else {
        return f;
      }
    case MAL_STRING:
      if (strcmp(first->as.string->data, second->as.string->data) == 0) {
        return t;
      } else {
        return f;
      }
    case MAL_HASHMAP:
      return hashmap_equal(this, first->as.hashmap, second->as.hashmap);
    case MAL_BOOLEAN:
      if (first->as.boolean == second->as.boolean) {
        return t;
      } else {
        return f;
      }
    case MAL_NIL:
      return t;
    case MAL_FUNCTION:
      if (first->as.function == second->as.function) {
        return t;
      } else {
        return f;
      }
    case MAL_CLOSURE:
      if (first->as.closure == second->as.closure) {
        return t;
      } else {
        return f;
      }
    default:
      return mal_error(this, ERROR_RUNTIME, text_make(this,
          "'=': this state should not happen\n"));
    }
  }
  return f;
}

mal_p core_lt(lvm_p this, mal_p args)
{
  mal_p first;
  mal_p second;
  mal_p t;
  mal_p f;
  int cmp;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (2 > args->as.list->count || (2 < args->as.list->count &&
      !is_nil(args->as.list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'<': expected exactly two arguments\n"));
  }
  first = args->as.list->data[0];
  second = args->as.list->data[1];
  if (!is_number(first) || !is_number(second)) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'<': expected numerical arguments\n"));
  }
  cmp = 0;
  if (is_integer(first) && is_integer(second)) {
    cmp = (first->as.integer < second->as.integer);
  } else if (is_integer(first) && is_decimal(second)) {
    cmp = (first->as.integer < second->as.decimal);
  } else if (is_decimal(first) && is_integer(second)) {
    cmp = (first->as.decimal < second->as.integer);
  } else if (is_decimal(first) && is_decimal(second)) {
    cmp = (first->as.decimal < second->as.decimal);
  }
  if (cmp) {
    return t;
  } else {
    return f;
  }
}

mal_p core_le(lvm_p this, mal_p args)
{
  mal_p first;
  mal_p second;
  mal_p t;
  mal_p f;
  int cmp;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (2 > args->as.list->count || (2 < args->as.list->count &&
      !is_nil(args->as.list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'<=': expected exactly two arguments\n"));
  }
  first = args->as.list->data[0];
  second = args->as.list->data[1];
  if (!is_number(first) || !is_number(second)) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'<=': expected numerical arguments\n"));
  }
  cmp = 0;
  if (is_integer(first) && is_integer(second)) {
    cmp = (first->as.integer <= second->as.integer);
  } else if (is_integer(first) && is_decimal(second)) {
    cmp = (first->as.integer <= second->as.decimal);
  } else if (is_decimal(first) && is_integer(second)) {
    cmp = (first->as.decimal <= second->as.integer);
  } else if (is_decimal(first) && is_decimal(second)) {
    cmp = (first->as.decimal <= second->as.decimal);
  }
  if (cmp) {
    return t;
  } else {
    return f;
  }
}

mal_p core_gt(lvm_p this, mal_p args)
{
  mal_p first;
  mal_p second;
  mal_p t;
  mal_p f;
  int cmp;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (2 > args->as.list->count || (2 < args->as.list->count &&
      !is_nil(args->as.list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'>': expected exactly two arguments\n"));
  }
  first = args->as.list->data[0];
  second = args->as.list->data[1];
  if (!is_number(first) || !is_number(second)) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'>': expected numerical arguments\n"));
  }
  cmp = 0;
  if (is_integer(first) && is_integer(second)) {
    cmp = (first->as.integer > second->as.integer);
  } else if (is_integer(first) && is_decimal(second)) {
    cmp = (first->as.integer > second->as.decimal);
  } else if (is_decimal(first) && is_integer(second)) {
    cmp = (first->as.decimal > second->as.integer);
  } else if (is_decimal(first) && is_decimal(second)) {
    cmp = (first->as.decimal > second->as.decimal);
  }
  if (cmp) {
    return t;
  } else {
    return f;
  }
}

mal_p core_ge(lvm_p this, mal_p args)
{
  mal_p first;
  mal_p second;
  mal_p t;
  mal_p f;
  int cmp;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if (2 > args->as.list->count || (2 < args->as.list->count &&
      !is_nil(args->as.list->data[2]))) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'>=': expected exactly two arguments\n"));
  }
  first = args->as.list->data[0];
  second = args->as.list->data[1];
  if (!is_number(first) || !is_number(second)) {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "'>=': expected numerical arguments\n"));
  }
  cmp = 0;
  if (is_integer(first) && is_integer(second)) {
    cmp = (first->as.integer >= second->as.integer);
  } else if (is_integer(first) && is_decimal(second)) {
    cmp = (first->as.integer >= second->as.decimal);
  } else if (is_decimal(first) && is_integer(second)) {
    cmp = (first->as.decimal >= second->as.integer);
  } else if (is_decimal(first) && is_decimal(second)) {
    cmp = (first->as.decimal >= second->as.decimal);
  }
  if (cmp) {
    return t;
  } else {
    return f;
  }
}

mal_p core_list(lvm_p this, mal_p args)
{
  list_p list = list_make(this, args->as.list->count);
  mal_p mal;
  size_t at;
  size_t in;
  mal_p nil;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  if (0 == args->as.list->count) {
    list_append(this, list, nil);
    return mal_list(this, list);
  }
  for (at = 0; at < args->as.list->count - 1; at++) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      for (in = 0; in < mal->as.list->count - 1; in++) {
        list_append(this, list, mal->as.list->data[in]);
      }
      if (!is_nil(mal->as.list->data[in])) {
        list_append(this, list, mal->as.list->data[in]);
      }
      break;
    case MAL_VECTOR:
      for (in = 0; in < mal->as.vector->count - 1; in++) {
        list_append(this, list, mal->as.vector->data[in]);
      }
      if (!is_nil(mal->as.vector->data[in])) {
        list_append(this, list, mal->as.vector->data[in]);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        list_append(this, list, mal->as.hashmap->data[in]);
      }
      break;
    default:
      list_append(this, list, mal);
    }
  }
  if (!is_nil(args->as.list->data[at])) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      for (in = 0; in < mal->as.list->count - 1; in++) {
        list_append(this, list, mal->as.list->data[in]);
      }
      if (!is_nil(mal->as.list->data[in])) {
        list_append(this, list, mal->as.list->data[in]);
      }
      break;
    case MAL_VECTOR:
      for (in = 0; in < mal->as.vector->count - 1; in++) {
        list_append(this, list, mal->as.vector->data[in]);
      }
      if (!is_nil(mal->as.vector->data[in])) {
        list_append(this, list, mal->as.vector->data[in]);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        list_append(this, list, mal->as.hashmap->data[in]);
      }
      break;
    default:
      list_append(this, list, mal);
    }
  }
  list_append(this, list, nil);
  return mal_list(this, list);
}

mal_p core_vector(lvm_p this, mal_p args)
{
  vector_p vector = vector_make(this, args->as.list->count);
  mal_p mal;
  size_t at;
  size_t in;
  mal_p nil;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  if (0 == args->as.list->count) {
    vector_append(this, vector, nil);
    return mal_vector(this, vector);
  }
  for (at = 0; at < args->as.list->count - 1; at++) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      for (in = 0; in < mal->as.list->count - 1; in++) {
        vector_append(this, vector, mal->as.list->data[in]);
      }
      if (!is_nil(mal->as.list->data[in])) {
        vector_append(this, vector, mal->as.list->data[in]);
      }
      break;
    case MAL_VECTOR:
      for (in = 0; in < mal->as.vector->count - 1; in++) {
        vector_append(this, vector, mal->as.vector->data[in]);
      }
      if (!is_nil(mal->as.vector->data[in])) {
        vector_append(this, vector, mal->as.vector->data[in]);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        vector_append(this, vector, mal->as.hashmap->data[in]);
      }
      break;
    default:
      vector_append(this, vector, mal);
    }
  }
  if (!is_nil(args->as.list->data[at])) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      for (in = 0; in < mal->as.list->count - 1; in++) {
        vector_append(this, vector, mal->as.list->data[in]);
      }
      if (!is_nil(mal->as.list->data[in])) {
        vector_append(this, vector, mal->as.list->data[in]);
      }
      break;
    case MAL_VECTOR:
      for (in = 0; in < mal->as.vector->count - 1; in++) {
        vector_append(this, vector, mal->as.vector->data[in]);
      }
      if (!is_nil(mal->as.vector->data[in])) {
        vector_append(this, vector, mal->as.vector->data[in]);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        vector_append(this, vector, mal->as.hashmap->data[in]);
      }
      break;
    default:
      vector_append(this, vector, mal);
    }
  }
  vector_append(this, vector, nil);
  return mal_vector(this, vector);
}

mal_p core_hashmap(lvm_p this, mal_p args)
{
  hashmap_p hashmap = hashmap_make(this, args->as.list->count);
  mal_p mal;
  mal_p nil;
  size_t at;
  size_t in;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  if (0 == args->as.list->count) {
    return mal_hashmap(this, hashmap);
  }
  for (at = 0; at < args->as.list->count - 1; at++) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      if (mal->as.list->count % 2 == 0) {
        for (in = 0; in < mal->as.list->count; in += 2) {
          hashmap_set(this, hashmap, mal->as.list->data[in],
              mal->as.list->data[in + 1]);
        }
      } else {
        for (in = 0; in < mal->as.list->count - 1; in += 2) {
          hashmap_set(this, hashmap, mal->as.list->data[in],
              mal->as.list->data[in + 1]);
        }
        hashmap_set(this, hashmap, mal->as.list->data[in], nil);
      }
      break;
    case MAL_VECTOR:
      if (mal->as.vector->count % 2 == 0) {
        for (in = 0; in < mal->as.vector->count; in += 2) {
          hashmap_set(this, hashmap, mal->as.vector->data[in],
              mal->as.vector->data[in + 1]);
        }
      } else {
        for (in = 0; in < mal->as.vector->count - 1; in += 2) {
          hashmap_set(this, hashmap, mal->as.vector->data[in],
              mal->as.vector->data[in + 1]);
        }
        hashmap_set(this, hashmap, mal->as.list->data[in], nil);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        hashmap_set(this, hashmap, mal->as.hashmap->data[in],
            mal->as.hashmap->data[in + 1]);
      }
      break;
    default:
      if (at + 1 != args->as.list->count - 1) {
        hashmap_set(this, hashmap, args->as.list->data[at],
            args->as.list->data[at + 1]);
        at++;
      } else {
        hashmap_set(this, hashmap, args->as.list->data[at], nil);
      }
    }
  }
  if (!is_nil(args->as.list->data[at])) {
    switch ((mal = args->as.list->data[at])->type) {
    case MAL_LIST:
      if (mal->as.list->count % 2 == 0) {
        for (in = 0; in < mal->as.list->count; in += 2) {
          hashmap_set(this, hashmap, mal->as.list->data[in],
              mal->as.list->data[in + 1]);
        }
      } else {
        for (in = 0; in < mal->as.list->count - 1; in += 2) {
          hashmap_set(this, hashmap, mal->as.list->data[in],
              mal->as.list->data[in + 1]);
        }
        hashmap_set(this, hashmap, mal->as.list->data[in], nil);
      }
      break;
    case MAL_VECTOR:
      if (mal->as.vector->count % 2 == 0) {
        for (in = 0; in < mal->as.vector->count; in += 2) {
          hashmap_set(this, hashmap, mal->as.vector->data[in],
              mal->as.vector->data[in + 1]);
        }
      } else {
        for (in = 0; in < mal->as.vector->count - 1; in += 2) {
          hashmap_set(this, hashmap, mal->as.vector->data[in],
              mal->as.vector->data[in + 1]);
        }
        hashmap_set(this, hashmap, mal->as.list->data[in], nil);
      }
      break;
    case MAL_HASHMAP:
      for (in = 0; in < mal->as.hashmap->count; in += 2) {
        hashmap_set(this, hashmap, mal->as.hashmap->data[in],
            mal->as.hashmap->data[in + 1]);
      }
      break;
    default:
      if (at + 1 != args->as.list->count - 1) {
        hashmap_set(this, hashmap, args->as.list->data[at],
            args->as.list->data[at + 1]);
        at++;
      } else {
        hashmap_set(this, hashmap, args->as.list->data[at], nil);
      }
    }
  }
  return mal_hashmap(this, hashmap);
}

mal_p core_zip(lvm_p this, mal_p args)
{
  size_t at;
  mal_p nil;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  if ((3 == args->as.list->count && is_nil(args->as.list->data[2])) ||
      2 == args->as.list->count) {
    if (is_sequential(args->as.list->data[0]) &&
        is_sequential(args->as.list->data[1])) {
      switch (args->as.list->data[0]->type) {
      case MAL_LIST:
        switch (args->as.list->data[1]->type) {
        case MAL_LIST:
          if (args->as.list->data[0]->as.list->count >=
              args->as.list->data[1]->as.list->count) {
            list_p list0 = args->as.list->data[0]->as.list;
            list_p list1 = args->as.list->data[1]->as.list;
            list_p result = list_make(this, list0->count << 1);

            for (at = 0; at < list1->count - 1; at++) {
              list_append(this, result, list0->data[at]);
              list_append(this, result, list1->data[at]);
            }
            if (!is_nil(list1->data[at])) {
              list_append(this, result, list0->data[at]);
              list_append(this, result, list1->data[at++]);
            }
            for (; at < list0->count; at++) {
              if (!is_nil(list0->data[at]) && at == list0->count - 1) {
                list_append(this, result, list0->data[at]);
                list_append(this, result, nil);
              }
            }
            list_append(this, result, nil);
            return mal_list(this, result);
          } else {
            return mal_error(this, ERROR_RUNTIME, text_make(this, "in zip "
                "first list has to be equal or longer then second list\n"));
          }
        case MAL_VECTOR:
          if (args->as.list->data[0]->as.list->count >=
              args->as.list->data[1]->as.vector->count) {
            list_p list0 = args->as.list->data[0]->as.list;
            vector_p vector1 = args->as.list->data[1]->as.vector;
            list_p result = list_make(this, list0->count << 1);

            for (at = 0; at < vector1->count - 1; at++) {
              list_append(this, result, list0->data[at]);
              list_append(this, result, vector1->data[at]);
            }
            if (!is_nil(vector1->data[at])) {
              list_append(this, result, list0->data[at]);
              list_append(this, result, vector1->data[at++]);
            }
            for (; at < list0->count; at++) {
              if (!is_nil(list0->data[at]) && at == list0->count - 1) {
                list_append(this, result, list0->data[at]);
                list_append(this, result, nil);
              }
            }
            list_append(this, result, nil);
            return mal_list(this, result);
          } else {
            return mal_error(this, ERROR_RUNTIME,  text_make(this, "in zip "
                "first list has to be equal or longer then second vector\n"));
          }
        default:
          return mal_error(this, ERROR_RUNTIME,
              text_concat(this, text_concat_text(this, text_make(this,
              "unsupported type of the second sequential '"), mal_print(this,
              args->as.list->data[0], false)), "'\n"));
        }
      case MAL_VECTOR:
        switch (args->as.list->data[1]->type) {
        case MAL_LIST:
          if (args->as.list->data[0]->as.vector->count >=
              args->as.list->data[1]->as.list->count) {
            vector_p vector0 = args->as.list->data[0]->as.vector;
            list_p list1 = args->as.list->data[1]->as.list;
            list_p result = list_make(this, vector0->count << 1);

            for (at = 0; at < list1->count - 1; at++) {
              list_append(this, result, vector0->data[at]);
              list_append(this, result, list1->data[at]);
            }
            if (!is_nil(list1->data[at])) {
              list_append(this, result, vector0->data[at]);
              list_append(this, result, list1->data[at++]);
            }
            for (; at < vector0->count; at++) {
              if (!is_nil(vector0->data[at]) && at == vector0->count - 1) {
                list_append(this, result, vector0->data[at]);
                list_append(this, result, nil);
              }
            }
            list_append(this, result, nil);
            return mal_list(this, result);
          } else {
            return mal_error(this, ERROR_RUNTIME, text_make(this, "in zip "
                "first vector has to be equal or longer then second list\n"));
          }
        case MAL_VECTOR:
          if (args->as.list->data[0]->as.vector->count >=
              args->as.list->data[1]->as.vector->count) {
            vector_p vector0 = args->as.list->data[0]->as.vector;
            vector_p vector1 = args->as.list->data[1]->as.vector;
            list_p result = list_make(this, vector0->count << 1);

            for (at = 0; at < vector1->count - 1; at++) {
              list_append(this, result, vector0->data[at]);
              list_append(this, result, vector1->data[at]);
            }
            if (!is_nil(vector1->data[at])) {
              list_append(this, result, vector0->data[at]);
              list_append(this, result, vector1->data[at++]);
            }
            for (; at < vector0->count; at++) {
              if (!is_nil(vector0->data[at]) && at == vector0->count - 1) {
                list_append(this, result, vector0->data[at]);
                list_append(this, result, nil);
              }
            }
            list_append(this, result, nil);
            return mal_list(this, result);
          } else {
            return mal_error(this, ERROR_RUNTIME, text_make(this, "in zip "
                "first vector has to be equal or longer then second vector\n"));
          }
        default:
          return mal_error(this, ERROR_RUNTIME,
              text_concat(this, text_concat_text(this, text_make(this,
              "unsupported type of the second sequential '"), mal_print(this,
              args->as.list->data[0], false)), "'\n"));
        }
      default:
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "unsupported type of the first sequential '"), mal_print(this,
            args->as.list->data[0], false)), "'\n"));
      }
    } else {
      return mal_error(this, ERROR_RUNTIME, text_make(this,
          "required two lists or vectors\n"));
    }
  } else {
    return mal_error(this, ERROR_RUNTIME, text_make(this,
        "required proper list of length 2\n"));
  }
}

mal_p core_listp(lvm_p this, mal_p args)
{
  mal_p nil;
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    if (is_list(args->as.list->data[0])) {
      return t;
    } else {
      return f;
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      if (is_list(args->as.list->data[at])) {
        vector_append(this, vector, t);
      } else {
        vector_append(this, vector, f);
      }
    }
    vector_append(this, vector, nil);
    return mal_vector(this, vector);
  }
  return f;
}

mal_p core_vectorp(lvm_p this, mal_p args)
{
  mal_p nil;
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    if (is_vector(args->as.list->data[0])) {
      return t;
    } else {
      return f;
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      if (is_vector(args->as.list->data[at])) {
        vector_append(this, vector, t);
      } else {
        vector_append(this, vector, f);
      }
    }
    vector_append(this, vector, nil);
    return mal_vector(this, vector);
  }
  return f;
}

mal_p core_hashmapp(lvm_p this, mal_p args)
{
  mal_p nil;
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    if (is_hashmap(args->as.list->data[0])) {
      return t;
    } else {
      return f;
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      if (is_hashmap(args->as.list->data[at])) {
        vector_append(this, vector, t);
      } else {
        vector_append(this, vector, f);
      }
    }
    vector_append(this, vector, nil);
    return mal_vector(this, vector);
  }
  return f;
}

mal_p core_envp(lvm_p this, mal_p args)
{
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    if (is_env(args->as.list->data[0])) {
      return t;
    } else {
      return f;
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      if (is_env(args->as.list->data[at])) {
        vector_append(this, vector, t);
      } else {
        vector_append(this, vector, f);
      }
    }
    return mal_vector(this, vector);
  }
  return f;
}

mal_p core_emptyp(lvm_p this, mal_p args)
{
  mal_p t;
  mal_p f;
  env_get_by_text(this, this->env, text_make(this, "boolean: true"), &t);
  env_get_by_text(this, this->env, text_make(this, "boolean: false"), &f);
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    switch (args->as.list->data[0]->type) {
    case MAL_LIST:
      if (0 == args->as.list->data[0]->as.list->count) {
        return t;
      } else {
        return f;
      }
    case MAL_VECTOR:
      if (0 == args->as.list->data[0]->as.vector->count) {
        return t;
      } else {
        return f;
      }
    case MAL_HASHMAP:
      if (0 == args->as.list->data[0]->as.hashmap->count) {
        return t;
      } else {
        return f;
      }
    case MAL_ENV:
      if (0 == args->as.list->data[0]->as.env->hashmap->count) {
        return t;
      } else {
        return f;
      }
    default:
      if (is_nil(args->as.list->data[0])) {
        return t;
      } else {
        return f;
      }
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      switch (args->as.list->data[at]->type) {
      case MAL_LIST:
        if (0 == args->as.list->data[at]->as.list->count) {
          vector_append(this, vector, t);
        } else {
          vector_append(this, vector, f);
        }
        break;
      case MAL_VECTOR:
        if (0 == args->as.list->data[at]->as.vector->count) {
          vector_append(this, vector, t);
        } else {
          vector_append(this, vector, f);
        }
        break;
      case MAL_HASHMAP:
        if (0 == args->as.list->data[at]->as.hashmap->count) {
          vector_append(this, vector, t);
        } else {
          vector_append(this, vector, f);
        }
        break;
      case MAL_ENV:
        if (0 == args->as.list->data[at]->as.env->hashmap->count) {
          vector_append(this, vector, t);
        } else {
          vector_append(this, vector, f);
        }
        break;
      default:
        if (is_nil(args->as.list->data[at])) {
          vector_append(this, vector, t);
        } else {
          vector_append(this, vector, f);
        }
        break;
      }
    }
    return mal_vector(this, vector);
  }
  return t;
}

mal_p core_count(lvm_p this, mal_p args)
{
  if ((args->as.list->count == 2 && is_nil(args->as.list->data[1])) ||
      (args->as.list->count == 1 && !is_nil(args->as.list->data[0]))) {
    switch (args->as.list->data[0]->type) {
    case MAL_LIST:
      return mal_integer(this, args->as.list->data[0]->as.list->count);
    case MAL_VECTOR:
      return mal_integer(this, args->as.list->data[0]->as.vector->count);
    case MAL_HASHMAP:
      return mal_integer(this, args->as.list->data[0]->as.hashmap->count >> 1);
    case MAL_ENV:
      return mal_integer(this,
          args->as.list->data[0]->as.env->hashmap->count >> 1);
    default:
      return mal_integer(this, 1);
    }
  } else if (args->as.list->count > 1 && !is_nil(args->as.list->data[1])) {
    size_t size = is_nil(args->as.list->data[args->as.list->count - 1]) ?
      args->as.list->count - 1: args->as.list->count;
    vector_p vector = vector_make(this, size);
    size_t at;
    for (at = 0; at < size; at++) {
      switch (args->as.list->data[at]->type) {
      case MAL_LIST:
        vector_append(this, vector, mal_integer(this,
            args->as.list->data[at]->as.list->count));
        break;
      case MAL_VECTOR:
        vector_append(this, vector, mal_integer(this,
            args->as.list->data[at]->as.vector->count));
        break;
      case MAL_HASHMAP:
        vector_append(this, vector, mal_integer(this,
            args->as.list->data[at]->as.hashmap->count >> 1));
        break;
      case MAL_ENV:
        vector_append(this, vector, mal_integer(this,
            args->as.list->data[at]->as.env->hashmap->count >> 1));
        break;
      default:
        vector_append(this, vector, mal_integer(this, 1));
        break;
      }
    }
    return mal_vector(this, vector);
  }
  return 0;
}

mal_p core_pr_str(lvm_p this, mal_p args)
{
  return mal_as_str(this, args, true, " ");
}

mal_p core_str(lvm_p this, mal_p args)
{
  return mal_as_str(this, args, false, "");
}

mal_p core_prn(lvm_p this, mal_p args)
{
  mal_p nil;
  char *str;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  printf("%s\n", str = lvm_print(this, mal_as_str(this, args, true, " ")));
  free((void *)str);
  return nil;
}

mal_p core_println(lvm_p this, mal_p args)
{
  mal_p nil;
  char *str;
  env_get_by_text(this, this->env, text_make(this, "nil: nil"), &nil);
  printf("%s\n", str = lvm_print(this, mal_as_str(this, args, false, " ")));
  free((void *)str);
  return nil;
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
  return read_str(this);
}

mal_p lvm_eval(lvm_p this, mal_p ast, env_p env)
{
  while (true) {
    mal_p evaluated;
    list_p params;
    mal_p callable;
    closure_p closure;
    list_p parameters;
    long arity;
    long arguments;
    list_p list;
    vector_p vector;
    hashmap_p hashmap;
    size_t at;
    size_t in;
    if (is_eoi(ast) || is_nil(ast)) {
      return ast;
    }
    if (!is_list(ast)) {
      return eval_ast(this, ast, env);
    }
    if (0 == ast->as.list->count) {
      return ast;
    }
    if (MAL_SYMBOL == ast->as.list->data[0]->type) {
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "def!")) {
        return eval_def_bang(this, ast, env);
      }
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "let*")) {
        ast = eval_let_star(this, ast, &env);
        if (is_error(ast)) {
          return ast;
        }
        continue;
      }
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "if")) {
        ast = eval_if(this, ast, &env);
        if (is_error(ast)) {
          return ast;
        }
        continue;
      }
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "fn*")) {
        return eval_fn_star(this, ast, env);
      }
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "do")) {
        ast = eval_do(this, ast, env);
        if (is_error(ast)) {
          return ast;
        }
        continue;
      }
      if (0 == text_cmp(this, ast->as.list->data[0]->as.symbol, "..")) {
        return mal_env(this, env);
      }
    }
    evaluated = eval_ast(this, ast, env);
    if (MAL_ERROR == evaluated->type) {
      return evaluated;
    }
    callable = evaluated->as.list->data[0];
    params = list_params(this, evaluated->as.list);
    switch (callable->type) {
    case MAL_SYMBOL:
      break;
    case MAL_FUNCTION:
      return (callable->as.function->definition)(this, mal_list(this, params));
    case MAL_CLOSURE:
      closure = callable->as.closure;
      parameters = closure->parameters->as.list;
      arity = parameters->count - 1;
      arguments = params->count - 1;
      if (arity > arguments) {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "'fn*': too few arguments supplied to the function '"),
            text_make_integer(this, arguments)), "'\n"));
      } else if ((arity < arguments) && !closure->more) {
        return mal_error(this, ERROR_RUNTIME,
            text_concat(this, text_concat_text(this, text_make(this,
            "'fn*': too many arguments supplied to the function '"),
            text_make_integer(this, arguments)), "'\n"));
      } else {
        env = env_make(this, closure->env, parameters, params,
            closure->more, 0);
        ast = callable->as.closure->definition;
        if (is_error(ast)) {
          return ast;
        }
        continue;
      }
    case MAL_HASHMAP:
      for (at = 0; at < params->count; at++) {
        if (is_nil(params->data[at]) && at == params->count - 1) {
          return callable;
        }
        switch (params->data[at]->type) {
        case MAL_LIST:
          list = params->data[at]->as.list;
          hashmap = hashmap_make(this, list->count << 1);
          for (in = 0; in < list->count - 1; in++) {
            hashmap_get(this, callable->as.hashmap, list->data[in], &evaluated);
            hashmap_set(this, hashmap, list->data[in], evaluated);
          }
          if (!is_nil(list->data[in])) {
            hashmap_get(this, callable->as.hashmap, list->data[in], &evaluated);
            hashmap_set(this, hashmap, list->data[in], evaluated);
          }
          callable = mal_hashmap(this, hashmap);
          break;
        case MAL_VECTOR:
          vector = params->data[at]->as.vector;
          hashmap = hashmap_make(this, vector->count << 1);
          for (in = 0; in < vector->count - 1; in++) {
            hashmap_get(this, callable->as.hashmap, vector->data[in], &evaluated);
            hashmap_set(this, hashmap, vector->data[in], evaluated);
          }
          if (!is_nil(vector->data[in])) {
            hashmap_get(this, callable->as.hashmap, vector->data[in], &evaluated);
            hashmap_set(this, hashmap, vector->data[in], evaluated);
          }
          callable = mal_hashmap(this, hashmap);
          break;
        default:
          if (!is_hashmap(callable) || !hashmap_get(this, callable->as.hashmap,
              params->data[at], &evaluated)) {
#if VAR_NIL
            env_get_by_text(this, env, text_make(this, "nil: nil"), &evaluated);
            return evaluated;
#else
            return mal_error(this, ERROR_RUNTIME,
                text_concat(this, text_concat_text(this, text_make(this,
                "var '"), mal_print(this, params->data[at], false)),
                "' not found\n"));
#endif
          }
          callable = evaluated;
          break;
        }
      }
      return evaluated;
    default:
      return mal_error(this, ERROR_RUNTIME,
          text_concat(this, text_concat_text(this, text_make(this,
          "first list item not callable '"), mal_print(this, callable, false)),
          "'\n"));
    }
    return ast;
  }
}

char *lvm_print(lvm_p this, mal_p value)
{
  char *output;
  if (this->error->count) {
    output = text_str(this, error_collapse(this));
  } else {
    output = text_str(this, mal_print(this, value, false));
  }
  return output;
}

char *lvm_rep(lvm_p this, char *str)
{
  return lvm_print(this, lvm_eval(this, lvm_read(this, str), this->env));
}

int main(int argc, char *argv[])
{
  lvm_p lvm = lvm_make();
  mal_p key;
  mal_p value;
  size_t at;
  typedef struct core_s {
    char *symbol;
    mal_p (*function)(lvm_p this, mal_p params);
  } core_t;
  core_t core[] = {
    {"+", core_add},
    {"-", core_sub},
    {"*", core_mul},
    {"/", core_div},
    {"=", core_eq},
    {"<", core_lt},
    {"<=", core_le},
    {">", core_gt},
    {">=", core_ge},
    {"list", core_list},
    {"vector", core_vector},
    {"hashmap", core_hashmap},
    {"zip", core_zip},
    {"list?", core_listp},
    {"vector?", core_vectorp},
    {"hashmap?", core_hashmapp},
    {"env?", core_envp},
    {"empty?", core_emptyp},
    {"count", core_count},
    {"pr-str", core_pr_str},
    {"str", core_str},
    {"prn", core_prn},
    {"println", core_println},
    {NULL, NULL}
  };
  (void)argc;
  (void)argv;
  puts("Make-a-lisp version 0.5.0\n");
  puts("Press Ctrl+D to exit\n");

  for (at = 0; core[at].symbol; at++) {
    key = mal_symbol(lvm, text_make(lvm, core[at].symbol));
    value = mal_function(lvm, function_make(lvm, core[at].function,
        key->identity));
    env_set(lvm, lvm->env, key, value);
  }
  lvm_eval(lvm, lvm_read(lvm, "(def! not (fn* (a) (if a false true)))"),
      lvm->env);
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
