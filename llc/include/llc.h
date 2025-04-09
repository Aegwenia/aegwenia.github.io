#ifndef LLC_H
#define LLC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void llc_void_t, *llc_void_p;
typedef char llc_i8_t, *llc_i8_p, **llc_i8_pp;
typedef unsigned char llc_u8_t, *llc_u8_p;
typedef short llc_i16_t, *llc_i16_p;
typedef unsigned short llc_u16_t, *llc_u16_p;
typedef int llc_i32_t, *llc_i32_p;
typedef unsigned int llc_u32_t, *llc_u32_p;
typedef long llc_i64_t, *llc_i64_p;
typedef unsigned long llc_u64_t, *llc_u64_p;

typedef enum {
  llc_boolean_false,
  llc_boolean_true
} llc_boolean_t;
#endif

#ifdef LLC_IMPLEMENTATION
#undef LLC_IMPLEMENTATION
#ifndef LLC_DEFINITION
#define LLC_DEFINITION
#else
#error "Multiple definitions of LLC_IMPLEMENTATION\n"
#endif

#endif

