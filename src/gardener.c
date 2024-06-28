// Let's imagine an app for managing plant's watering.

/*******************************************************************************
 *    INCLUDES
 ******************************************************************************/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// App
#include "gardener.h"
#include "interfaces/std_lib_interface.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE API DECLARATIONS
 ******************************************************************************/
static bool is_watering_required(plant *plant_, unsigned long time);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
plant *create_plant(char *species, float water_amount, unsigned long start_date,
                    unsigned long last_watering_date,
                    unsigned long watering_period) {

  plant *p = app_malloc(sizeof(plant));
  if (!p)
    return NULL;

  strcpy(p->species, species);
  p->water_amount = water_amount;
  p->last_watering_date = last_watering_date;
  p->watering_period = watering_period;
  p->start_date = start_date;

  return p;
}

bool water_plant(plant *plant_) {
  // If watering done return `true`, otherwise `false`.

  unsigned long now = get_current_time();

  if (!is_watering_required(plant_, now))
    return false;

  plant_->last_watering_date = now;

  return true;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static bool is_watering_required(plant *plant_, unsigned long time) {
  // Detect overflow
  if (plant_->last_watering_date > (ULONG_MAX - plant_->watering_period) ||
      (plant_->watering_period > (ULONG_MAX - plant_->last_watering_date)))
    app_exit(2);

  unsigned long new_watering_period =
      plant_->last_watering_date + plant_->watering_period;

  return time > new_watering_period;
}
