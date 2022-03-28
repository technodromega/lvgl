/*******************************************************************************
 *    INCLUDE SECTION
 ******************************************************************************/
#include "lv_ft6236.h"

#include <openthread/cli.h>
#include "sl_ot_freertos_adaptation.h"


/*******************************************************************************
 *    DEFINE MACRO SECTION
 ******************************************************************************/
#define CONFIG_LV_FT6X36_SWAPXY      1
#define CONFIG_LV_FT6X36_INVERT_X    0
#define CONFIG_LV_FT6X36_INVERT_Y    1


/*******************************************************************************
 *    TYPE DEFINITION SECTION
 ******************************************************************************/
typedef struct
{
    int16_t last_x;
    int16_t last_y;
    lv_indev_state_t current_state;
} lv_ft6x36_touch_t;


/*******************************************************************************
 *    VARIABLE DEFINITION SECTION
 ******************************************************************************/
static lv_ft6x36_touch_t touch_inputs = { 0, 0, LV_INDEV_STATE_REL }; // Set coordinates to 0 by default


/*******************************************************************************
 *    FUNCTION DEFINITION SECTION
 ******************************************************************************/
FT6x36Error_t lv_ft6236_init() {
    FT6x36Error_t ret;
    ret = ft6236_init();
    if (ret == FT6x36_NO_ERROR) {
        FT6x36Info_t *dev_info;
        dev_info = ft6236_get_device_info();
        if (dev_info != NULL) {
            OT_API_CALL(otCliOutputFormat("(DEBUG) (%s): DEVICE ID:    0x%02x\r\n", pcTaskGetName(NULL), dev_info->device_id));
            OT_API_CALL(otCliOutputFormat("(DEBUG) (%s): CHIP ID:      0x%02x\r\n", pcTaskGetName(NULL), dev_info->chip_id));
            OT_API_CALL(otCliOutputFormat("(DEBUG) (%s): FIRMWARE ID:  0x%02x\r\n", pcTaskGetName(NULL), dev_info->firmware_id));
            OT_API_CALL(otCliOutputFormat("(DEBUG) (%s): RELEASE CODE: 0x%02x\r\n", pcTaskGetName(NULL), dev_info->release_code));
        }
    }

    return ret;
}

void lv_ft6236_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    uint8_t data_buf[5] = {0, 0, 0, 0, 0}; // 1 byte status, 2 bytes X, 2 bytes Y

    FT6x36Error_t ret = ft6236_i2c_read(FT6X36_TD_STAT_REG, &data_buf[0], 5);
    if (ret != FT6x36_NO_ERROR) {
        OT_API_CALL(otCliOutputFormat(ANSI_COLOR_RED "(ERROR) (%s): Error talking to touch IC: %d" ANSI_COLOR_RESET "\r\n", pcTaskGetName(NULL), ret));
    }

    uint8_t touch_pnt_cnt = data_buf[0]; // Number of detected touch points

    if (ret != FT6x36_NO_ERROR || touch_pnt_cnt != 1) { // Ignore no touch & multi touch
        if ( touch_inputs.current_state != LV_INDEV_STATE_REL) {
            touch_inputs.current_state = LV_INDEV_STATE_REL;
        }
        data->point.x = touch_inputs.last_x;
        data->point.y = touch_inputs.last_y;
        data->state = touch_inputs.current_state;
        return;
    }

    touch_inputs.current_state = LV_INDEV_STATE_PR;
    touch_inputs.last_x = ((data_buf[1] & FT6X36_MSB_MASK) << 8) | (data_buf[2] & FT6X36_LSB_MASK);
    touch_inputs.last_y = ((data_buf[3] & FT6X36_MSB_MASK) << 8) | (data_buf[4] & FT6X36_LSB_MASK);

#if CONFIG_LV_FT6X36_SWAPXY
    int16_t swap_buf = touch_inputs.last_x;
    touch_inputs.last_x = touch_inputs.last_y;
    touch_inputs.last_y = swap_buf;
#endif
#if CONFIG_LV_FT6X36_INVERT_X
    touch_inputs.last_x =  LV_HOR_RES - touch_inputs.last_x;
#endif
#if CONFIG_LV_FT6X36_INVERT_Y
    touch_inputs.last_y = LV_VER_RES - touch_inputs.last_y;
#endif
    data->point.x = touch_inputs.last_x;
    data->point.y = touch_inputs.last_y;
    data->state = touch_inputs.current_state;
    OT_API_CALL(otCliOutputFormat("(DEBUG) (%s): X = %u Y = %u\r\n", pcTaskGetName(NULL), data->point.x, data->point.y));
}
