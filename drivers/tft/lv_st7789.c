/*******************************************************************************
 *    INCLUDE SECTION
 ******************************************************************************/
#include "lv_st7789.h"
#include <stdint.h>


/*******************************************************************************
 *    DEFINE MACRO SECTION
 ******************************************************************************/
#define CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT               1
#define CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT_INVERTED      0
#define CONFIG_LV_DISPLAY_ORIENTATION_LANDSCAPE              0
#define CONFIG_LV_DISPLAY_ORIENTATION_LANDSCAPE_INVERTED     0

#define CONFIG_LV_TFT_DISPLAY_OFFSETS                        0
#if (CONFIG_LV_TFT_DISPLAY_OFFSETS)
#define CONFIG_LV_TFT_DISPLAY_X_OFFSET                       0
#define CONFIG_LV_TFT_DISPLAY_Y_OFFSET                       0
#endif


/*******************************************************************************
 *    FUNCTION DEFINITION SECTION
 ******************************************************************************/
void lv_st7789_init(void)
{
    st7789_init();
}

/* The ST7789 display controller can drive up to 320*240 displays, when using a 240*240 or 240*135
 * displays there's a gap of 80px or 40/52/53px respectively. 52px or 53x offset depends on display orientation.
 * We need to edit the coordinates to take into account those gaps, this is not necessary in all orientations. */
void lv_st7789_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    uint16_t offsetx1 = area->x1;
    uint16_t offsetx2 = area->x2;
    uint16_t offsety1 = area->y1;
    uint16_t offsety2 = area->y2;

#if (CONFIG_LV_TFT_DISPLAY_OFFSETS)
    offsetx1 += CONFIG_LV_TFT_DISPLAY_X_OFFSET;
    offsetx2 += CONFIG_LV_TFT_DISPLAY_X_OFFSET;
    offsety1 += CONFIG_LV_TFT_DISPLAY_Y_OFFSET;
    offsety2 += CONFIG_LV_TFT_DISPLAY_Y_OFFSET;

#elif (LV_HOR_RES_MAX == 240) && (LV_VER_RES_MAX == 240)
    #if (CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT)
        offsetx1 += 80;
        offsetx2 += 80;
    #elif (CONFIG_LV_DISPLAY_ORIENTATION_LANDSCAPE_INVERTED)
        offsety1 += 80;
        offsety2 += 80;
    #endif
#elif (LV_HOR_RES_MAX == 240) && (LV_VER_RES_MAX == 135)
    #if (CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT) || \
        (CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT_INVERTED)
        offsetx1 += 40;
        offsetx2 += 40;
        offsety1 += 53;
        offsety2 += 53;
    #endif
#elif (LV_HOR_RES_MAX == 135) && (LV_VER_RES_MAX == 240)
    #if (CONFIG_LV_DISPLAY_ORIENTATION_LANDSCAPE) || \
        (CONFIG_LV_DISPLAY_ORIENTATION_LANDSCAPE_INVERTED)
        offsetx1 += 52;
        offsetx2 += 52;
        offsety1 += 40;
        offsety2 += 40;
    #endif
#endif

    /*Column addresses*/
    st7789_writeCommand(ST7789_CASET);
    st7789_writeData((offsetx1 >> 8) & 0xFF);
    st7789_writeData(offsetx1 & 0xFF);
    st7789_writeData((offsetx2 >> 8) & 0xFF);
    st7789_writeData(offsetx2 & 0xFF);

    /*Page addresses*/
    st7789_writeCommand(ST7789_RASET);
    st7789_writeData((offsety1 >> 8) & 0xFF);
    st7789_writeData(offsety1 & 0xFF);
    st7789_writeData((offsety2 >> 8) & 0xFF);
    st7789_writeData(offsety2 & 0xFF);

    /*Memory write*/
    st7789_writeCommand(ST7789_RAMWR);

    int16_t size = (int16_t)lv_area_get_width(area) * (int16_t)lv_area_get_height(area);

    for (int i=0; i<size; i++) {
        uint16_t* color_p = (uint16_t*)(color_map + i);
        uint16_t color = *color_p;
        st7789_writeData(color);
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(drv);
}
