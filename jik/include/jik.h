#ifndef JIK_H
#define JIK_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void jik_void_t, *jik_void_p;
typedef char jik_i8_t, *jik_i8_p, **jik_i8_pp;
typedef unsigned char jik_u8_t, *jik_u8_p, **jik_u8_pp;
typedef short jik_i16_t, *jik_i16_p, **jik_i16_pp;
typedef unsigned short jik_u16_t, *jik_u16_p, **jik_u16_pp;
typedef int jik_i32_t, *jik_i32_p, **jik_i32_pp;
typedef unsigned int jik_u32_t, *jik_u32_p, **jik_u32_pp;
typedef long jik_i64_t, *jik_i64_p, **jik_i64_pp;
typedef unsigned long jik_u64_t, *jik_u64_p, **jik_u64_pp;
typedef float jik_f32_t, *jik_f32_p, **jik_f32_pp;
typedef double jik_f64_t, *jik_f64_p, **jik_f64_pp;

typedef enum {
  jik_boolean_false,
  jik_boolean_true
} jik_boolean_t;
#endif

#ifdef JIK_IMPLEMENTATION
#undef JIK_IMPLEMENTATION
#ifndef JIK_DEFINITION
#define JIK_DEFINITION
#else
#error "Multiple definitions of JIK_IMPLEMENTATION\n"
#endif

#endif
