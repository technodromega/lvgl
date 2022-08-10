/*******************************************************************************
 *    INCLUDE SECTION
 ******************************************************************************/
#include "lv_st7789.h"

#include <stdint.h>


/*******************************************************************************
 *    DEFINE MACRO SECTION
 ******************************************************************************/
#define LVGL_TFT_CONFIG_ORIENTATION_PORTRAIT               1
#define LVGL_TFT_CONFIG_ORIENTATION_PORTRAIT_INVERTED      0
#define LVGL_TFT_CONFIG_ORIENTATION_LANDSCAPE              0
#define LVGL_TFT_CONFIG_ORIENTATION_LANDSCAPE_INVERTED     0

#define LVGL_TFT_CONFIG_DISPLAY_OFFSETS                    0
#if (LVGL_TFT_CONFIG_DISPLAY_OFFSETS)
#define LVGL_TFT_CONFIG_DISPLAY_X_OFFSET                   0
#define LVGL_TFT_CONFIG_DISPLAY_Y_OFFSET                   0
#endif


/*******************************************************************************
 *    FUNCTION DEFINITION SECTION
 ******************************************************************************/
void LVGL_TftInit()
{
    ST7789_Init();
}

void LVGL_TftFlush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* colorMap)
{
    /* The ST7789 display controller can drive up to 320*240 displays, when using a 240*240 or 240*135
     * displays there is a gap of 80px or 40/52/53px respectively. 52px or 53x offset depends on display orientation.
     * We need to edit the coordinates to take into account those gaps, this is not necessary in all orientations. */
    uint16_t offsetx1 = area->x1;
    uint16_t offsetx2 = area->x2;
    uint16_t offsety1 = area->y1;
    uint16_t offsety2 = area->y2;

#if (LVGL_TFT_CONFIG_DISPLAY_OFFSETS)
    offsetx1 += LVGL_TFT_CONFIG_DISPLAY_X_OFFSET;
    offsetx2 += LVGL_TFT_CONFIG_DISPLAY_X_OFFSET;
    offsety1 += LVGL_TFT_CONFIG_DISPLAY_Y_OFFSET;
    offsety2 += LVGL_TFT_CONFIG_DISPLAY_Y_OFFSET;

#elif (LV_HOR_RES_MAX == 240) && (LV_VER_RES_MAX == 240)
    #if (LVGL_TFT_CONFIG_ORIENTATION_PORTRAIT)
        offsetx1 += 80;
        offsetx2 += 80;
    #elif (LVGL_TFT_CONFIG_ORIENTATION_LANDSCAPE_INVERTED)
        offsety1 += 80;
        offsety2 += 80;
    #endif
#elif (LV_HOR_RES_MAX == 240) && (LV_VER_RES_MAX == 135)
    #if (LVGL_TFT_CONFIG_ORIENTATION_PORTRAIT) || \
        (LVGL_TFT_CONFIG_ORIENTATION_PORTRAIT_INVERTED)
        offsetx1 += 40;
        offsetx2 += 40;
        offsety1 += 53;
        offsety2 += 53;
    #endif
#elif (LV_HOR_RES_MAX == 135) && (LV_VER_RES_MAX == 240)
    #if (LVGL_TFT_CONFIG_ORIENTATION_LANDSCAPE) || \
        (LVGL_TFT_CONFIG_ORIENTATION_LANDSCAPE_INVERTED)
        offsetx1 += 52;
        offsetx2 += 52;
        offsety1 += 40;
        offsety2 += 40;
    #endif
#endif

    /*Column addresses*/
    ST7789_SendCommand(ST7789_CASET);
    ST7789_SendData((offsetx1 >> 8) & 0xFF);
    ST7789_SendData(offsetx1 & 0xFF);
    ST7789_SendData((offsetx2 >> 8) & 0xFF);
    ST7789_SendData(offsetx2 & 0xFF);

    /*Page addresses*/
    ST7789_SendCommand(ST7789_RASET);
    ST7789_SendData((offsety1 >> 8) & 0xFF);
    ST7789_SendData(offsety1 & 0xFF);
    ST7789_SendData((offsety2 >> 8) & 0xFF);
    ST7789_SendData(offsety2 & 0xFF);

    /*Memory write*/
    ST7789_SendCommand(ST7789_RAMWR);

    int16_t size = (int16_t)lv_area_get_width(area) * (int16_t)lv_area_get_height(area);

    for (int i = 0; i < size; i++) {
        uint16_t* pColor = (uint16_t*)(colorMap + i);
        uint16_t color = *pColor;
        ST7789_SendData(color);
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(drv);
}
