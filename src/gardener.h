#ifndef _gardener_h
#define _gardener_h

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C Standard library
#include <stdbool.h>

/*******************************************************************************
 *    DATA STRUCTURES
 ******************************************************************************/
typedef struct plant {
  char species[1024];

  // In liters
  float water_amount;

  // In seconds since epoch
  unsigned long start_date;

  // In seconds since epoch
  unsigned long last_watering_date;

  // In seconds since epoch
  unsigned long watering_period;

} plant;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
plant *create_plant(char *species, float water_amount, unsigned long start_date,
                    unsigned long last_watering_date,
                    unsigned long watering_period);
bool water_plant(plant *plant_);

#endif
