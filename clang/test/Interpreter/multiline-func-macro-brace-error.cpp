// REQUIRES: host-supports-jit
// UNSUPPORTED: system-aix
// RUN: cat %s | clang-repl 2>&1 | FileCheck %s
// Check invalid brace counts are detected.
// Brace counting is done to support multiline functions.
extern "C" int printf(const char *, ...);

#define ___DEFINED_SYMBOL

int test_multiline_brace_count_error() {
#ifdef ___UNDEFINED_SYMBOL
  printf("Simple multiline brace count\n");
#else
}
#endif
}
// CHECK: error: Parsing failed. Unmathced braces.

#define A 1
#define B 3
#define C (A + B)
#define D C

int test_multiline_brace_count_complex_error() {
#if A + B - 2 * C + D == 0
}
#else
  printf("Complex multiline brace count\n");
#endif
}
// CHECK-NEXT: error: Parsing failed. Unmathced braces.

% quit
