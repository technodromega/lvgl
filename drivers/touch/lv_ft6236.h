#ifndef LV_FT6236_H
#define LV_FT6236_H

/*********************************
 *        INCLUDE SECTION
 *********************************/
#include "ft6236.h"
#include "lvgl.h"


/*********************************
 *  FUNCTION DECLARATION SECTION
 ********************************/
/**
  * @brief  Initialize touch controller communication via I2C
  * @retval Initialization status, 0 on success
  */
FT6236Error_t LVGL_TouchInit();

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv: Touch driver
  * @param  data: Store data here
  */
void LVGL_TouchRead(lv_indev_drv_t* drv, lv_indev_data_t* data);

#endif /* LV_FT6X06_H */
