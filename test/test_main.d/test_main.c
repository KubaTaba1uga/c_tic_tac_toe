#include <signal.h>
#include <stdio.h>

#include <unity.h>

#include "game/game.h"
#include "input/input.h"
#include "main.h"

static void suppress_signal(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_IGN; // Ignore the signal
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(sig, &sa, NULL) == -1) {
    perror("sigaction");
  }
}

static void restore_signal(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL; // Restore the default handler
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(sig, &sa, NULL) == -1) {
    perror("sigaction");
  }
}

static int input_wait_mock(void) { return 0; };

void setUp() {
  struct InputOps *input_ops = get_input_ops();
  input_ops->wait = input_wait_mock;
  suppress_signal(SIGUSR1);
}

void tearDown() {
  restore_signal(SIGUSR1); // Restore default handling of SIGUSR1
}

void test_main(void) {
  int result = main__();

  TEST_ASSERT_EQUAL(result, 0);
  TEST_PASS_MESSAGE("main is working.");
}
