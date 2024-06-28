/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
// Standard library
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Test framework
#include "unity.h"

// App
/* Include *.c instead of *.h to test static functions. */
#include "gardener.c"
#include "mock_std_lib_interface.h"
#include "mock_std_lib_utils.h"
/*******************************************************************************
 *    DATA
 ******************************************************************************/
char species[] = "Renanthera monachica";
float water_amount = 0.2;
unsigned long watering_period = 86400, start_date = 0, last_watering_date = 0;

plant *p = NULL;
size_t plant_size = sizeof(plant);

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void) {
  void *memory_mock;

  memory_mock = malloc(plant_size);
  if (!memory_mock)
    TEST_FAIL_MESSAGE("Unable to allocate memory. Mocking malloc failed!");

  app_malloc_ExpectAndReturn(plant_size, memory_mock);

  p = create_plant(species, water_amount, start_date, last_watering_date,
                   watering_period);
}

void tearDown(void) {
  free(p);
  p = NULL;
}
/*******************************************************************************
 *    PUBLIC API TESTS
 ******************************************************************************/

void test_create_plant_success(void) {
  TEST_ASSERT_NOT_NULL(p);

  TEST_ASSERT_EQUAL_STRING(species, p->species);

  TEST_ASSERT_EQUAL_FLOAT(water_amount, p->water_amount);

  TEST_ASSERT_EQUAL(start_date, p->start_date);
  TEST_ASSERT_EQUAL(last_watering_date, p->last_watering_date);
  TEST_ASSERT_EQUAL(watering_period, p->watering_period);
}

void test_create_plant_failure(void) {
  plant *p;

  app_malloc_IgnoreAndReturn(NULL);

  p = create_plant(species, water_amount, start_date, last_watering_date,
                   watering_period);

  TEST_ASSERT_NULL(p);
}

void test_water_plant(void) {
  bool received;
  unsigned long now_mock = last_watering_date + watering_period;

  get_current_time_IgnoreAndReturn(now_mock + 1);

  received = water_plant(p);

  TEST_ASSERT_TRUE(received);
  TEST_ASSERT_TRUE(p->last_watering_date > now_mock);
}

/*******************************************************************************
 *    PRIVATE API TESTS
 ******************************************************************************/
void test_is_watering_required_true_false(void) {
  bool received;
  unsigned long now_mock = last_watering_date + watering_period;

  //
  /* Test case #0 */
  received = is_watering_required(p, now_mock);

  TEST_ASSERT_FALSE(received);

  //
  /* Test case #1 */
  received = is_watering_required(p, now_mock - 1);

  TEST_ASSERT_FALSE(received);

  //
  /* Test case #2 */
  received = is_watering_required(p, now_mock + 1);

  TEST_ASSERT_TRUE(received);
}

void test_is_watering_required_overflow(void) {
  // App exits if overflow
  p->watering_period = ULONG_MAX;
  p->last_watering_date = 1;

  /* Make sure exit was called */
  app_exit_ExpectAndReturn(2, 999);
  is_watering_required(p, 0);
}
