// REQUIRES: host-supports-jit
// UNSUPPORTED: system-aix
// RUN: cat %s | clang-repl | FileCheck %s
// Check a brace counting is not broken by introduction of macro directives.
// Brace counting is done to support multiline functions.
extern "C" int printf(const char *, ...);

#define ___DEFINED_SYMBOL

int test_multiline_brace_count_simple() {
#ifdef ___UNDEFINED_SYMBOL
  {
#else
  printf("Simple multiline brace count\n");
#endif
    return 0;
  }
  auto r1 = test_multiline_brace_count_simple();
  // CHECK: Simple multiline brace count

  int test_multiline_brace_count_defined() {
#ifdef ___DEFINED_SYMBOL
    printf("Defined multiline brace count\n");
#else
}
}
}
}
}
}
#endif
    return 0;
  }
  auto r2 = test_multiline_brace_count_defined();
  // CHECK-NEXT: Defined multiline brace count

#define A 1
#define B 3
#define C (A + B)
#define D C

  int test_multiline_brace_count_complex() {
#if A + B - 2 * C + D == 0
    printf("Complex multiline brace count\n");
#else
  {
#endif
    return 0;
  }
  auto r3 = test_multiline_brace_count_complex();
  // CHECK-NEXT: Complex multiline brace count

  % quit
