#include <unity.h>

#include "input/input.h"
#include "main.c"

void input_wait_mock(void){};

void setUp() {
  struct InputOps *input_ops = get_input_ops();

  input_ops->wait = input_wait_mock;
}

void test_main(void) {
  int result = main__();

  TEST_ASSERT_EQUAL(result, 0);
  TEST_PASS_MESSAGE("main is working.");
}
