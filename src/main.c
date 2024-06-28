/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gardener.h"
#include "utils/logging_utils.h"
/*******************************************************************************
 *    DATA STRUCTURES
 ******************************************************************************/
plant my_plants_[] = {{.species = "Renanthera monachica",
                       .water_amount = 0.2,
                       .watering_period = 86400,
                       .start_date = 123,
                       .last_watering_date = 246},
                      {.species = "Arachnis annamensis",
                       .water_amount = 0.15,
                       .watering_period = 43200,
                       .start_date = 246,
                       .last_watering_date = 86400}};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  size_t i, plants_amount = sizeof(my_plants_) / sizeof(plant);
  bool was_watered;

  // init logging
  init_loggers();

  for (i = 0; i < plants_amount; i++) {
    was_watered = water_plant(&my_plants_[i]);

    if (was_watered)
      log_info("main", "%s was watered.", my_plants_[i].species);
    else
      log_info("main", "%s wasn't watered.", my_plants_[i].species);
  }

  // cleanup logging
  destroy_loggers();

  return 0;
}
