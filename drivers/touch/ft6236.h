#ifndef FT6236_H
#define FT6236_H

/*********************************
 *        INCLUDE SECTION
 *********************************/
#include "ft6236_driver.h"
#include "lvgl.h"


/*********************************
 *  FUNCTION DECLARATION SECTION
 ********************************/
/**
  * @brief  Initialize for FT6x36 communication via I2C
  * @retval Initialization status, 0 on success
  */
FT6x36Error_t lv_ft6236_init();

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv: Touch driver
  * @param  data: Store data here
  */
void lv_ft6236_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#endif /* FT6X06_H */
