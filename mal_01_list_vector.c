#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct lvm_s;
typedef struct lvm_s lvm_t, *lvm_p, **lvm_pp;

struct lvm_s {
  struct {
    char *str;
    size_t pos;
    size_t line;
    size_t column;
  } reader;
};

#if defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__NT__)
char *strndup(char *str, size_t n)
#endif
char *readline(lvm_p this, char *prompt);
lvm_p lvm_make();
char *lvm_read(lvm_p this, char *str);
char *lvm_eval(lvm_p this, char *str);
char *lvm_print(lvm_p this, char *str);
char *lvm_rep(lvm_p this, char *str);

#if defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__NT__)
char *strndup(char *str, size_t n)
{
  char *buffer;
  int i;

  buffer = (char *) malloc(n + 1);
  if (buffer) {
    for (i = 0; (i < n) && (str[i] != 0); i++) {
      buffer[n] = str[n];
    }
    buffer[i] = 0x00;
  }

  return buffer;
}
#endif

char *readline(lvm_p this, char *prompt)
{
  char buffer[2048], *tmp;
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
  lvm->reader.str = NULL;
  lvm->reader.pos = 0;
  lvm->reader.line = 1;
  lvm->reader.column = 0;
  return lvm;
}

void lvm_free(lvm_pp this)
{
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
  return str;
}

char *lvm_print(lvm_p this, char *str)
{
  char *output = strdup(str);
  return output;
}

char *lvm_rep(lvm_p this, char *str)
{
  return lvm_print(this, lvm_eval(this, lvm_read(this, str)));
}

int main(int argc, char *argv[])
{
  lvm_p lvm = lvm_make();
  puts("Make-a-lisp version 0.0.0\n");
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
    free((void *)input);
  }
  lvm_free(&lvm);
  return 0;
}

