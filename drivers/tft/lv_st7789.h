#ifndef LV_ST7789_H
#define LV_ST7789_H

/*********************************
 *        INCLUDE SECTION
 *********************************/
#include "st7789.h"
#include "lvgl.h"


/*********************************
 *  FUNCTION DECLARATION SECTION
 ********************************/
/**
  * @brief  Initialize the ST7789 TFT display controller
  */
void LVGL_TftInit();

/**
  * @brief  Draw pixels on screen
  * @param  drv: Pointer to TFT driver
  * @param  area: Pointer to area to draw
  * @param  color_map: Pointer to pixels
  */
void LVGL_TftFlush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* colorMap);

#endif /* LV_ST7789_H  */
