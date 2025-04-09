#define MAL_IMPLEMENTATION
#include <mal.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  mal_lvm_t lvm;
  mal_lvm_init(&lvm);
  mal_lvm_repl(&lvm, "mal");
  mal_lvm_cleanup(&lvm);
  return 0;
}
