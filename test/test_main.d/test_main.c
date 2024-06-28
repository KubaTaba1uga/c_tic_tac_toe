#include <unity.h>

#include "main.c"

void test_main(void) {
  int result = main__();

  TEST_ASSERT_EQUAL(result, 0);
  TEST_PASS_MESSAGE("main is working.");
}
