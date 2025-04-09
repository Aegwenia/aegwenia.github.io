#define CORE_IMPLEMENTATION
#include <core.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  core_lvm_t lvm;
  core_token_p token;
  core_lvm_init(&lvm);
  core_lvm_string(&lvm, "i32_t main(i32_t argc, string_t argv[])"
      " /* /* comment */ */\n{\n  return 0; // comment\n}\n");
  while ((token = core_reader_scan(&lvm, core_lvm_reader(&lvm)))) {
    if (CORE_TOKEN_EOI == token->kind) {
      break;
    }
  }
  fprintf(stdout, "Tokens:\n");
  token = lvm.reader.first;
  while (token) {
    core_token_print(&lvm, token);
    token = token->next;
  }
  core_lvm_cleanup(&lvm);
  return 0;
}
