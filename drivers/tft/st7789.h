#ifndef ST7789_H
#define ST7789_H

/*********************************
 *        INCLUDE SECTION
 *********************************/
#include "lvgl.h"


/*********************************
 *  FUNCTION DECLARATION SECTION
 ********************************/
/**
  * @brief  Initialize the ST7789 TFT display controller
  */
void lv_st7789_init(void);

/**
  * @brief  Draw pixels on screen
  * @param  drv: Pointer to TFT driver
  * @param  area: Pointer to area to draw
  * @param  color_map: Pointer to pixels
  */
void lv_st7789_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

#endif /* ST7789_H  */
