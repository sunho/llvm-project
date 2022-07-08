// REQUIRES: host-supports-jit
// UNSUPPORTED: system-aix
// RUN: cat %s | clang-repl | FileCheck %s
// Check a multiline function is parsed and executed correctly.
extern "C" int printf(const char *, ...);
int test_multiline_function() {
  printf("Multiline\n");
  printf("Function\n");
  return 0;
}

auto r1 = test_multiline_function();
// CHECK: Multiline
// CHECK-NEXT: Function

% quit
