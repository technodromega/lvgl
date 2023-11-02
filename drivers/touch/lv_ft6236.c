/*******************************************************************************
 *    INCLUDE SECTION
 ******************************************************************************/
#include "lv_ft6236.h"
#include "ft6236_calibration.h"

#include <stdio.h>

#include <FreeRTOS.h>
#include "task.h"
#include "sl_sleeptimer.h"
#include "../../../examples/gcp-coaps-demo/efr32/src/display/interface/display.h"


/*******************************************************************************
 *    DEFINE MACRO SECTION
 ******************************************************************************/
#define LVGL_TOUCH_CONFIG_SWAPXY      1
#define LVGL_TOUCH_CONFIG_INVERT_X    0
#define LVGL_TOUCH_CONFIG_INVERT_Y    1

#if FT6236_CALIBRATION_REQUIRED == 1
#define LVGL_TOUCH_CALIB_REGION       2 * LV_VER_RES / 3
#define LVGL_TOUCH_X_OFFSET           0
#define LVGL_TOUCH_Y_OFFSET           30
#endif


/*******************************************************************************
 *    TYPE DEFINITION SECTION
 ******************************************************************************/
typedef struct LvglTouchCoordinates {
    int16_t lastX;
    int16_t lastY;
    lv_indev_state_t currentState;
} LvglTouchCoordinates_t;


/*******************************************************************************
 *    VARIABLE DEFINITION SECTION
 ******************************************************************************/
static LvglTouchCoordinates_t lvgl_touchInputs = {0, 0, LV_INDEV_STATE_REL}; // Set coordinates to 0 by default


/*******************************************************************************
 *    FUNCTION DEFINITION SECTION
 ******************************************************************************/
FT6236Error_t LVGL_TouchInit()
{
    FT6236Error_t ret;
    ret = FT6236_Init();
    if (ret == FT6236_NO_ERROR) {
        FT6236Info_t *devInfo;
        devInfo = FT6236_GetDeviceInfo();
        if (devInfo != NULL) {
            printf("(DEBUG) (%s): DEVICE ID:    0x%02x\r\n", pcTaskGetName(NULL), devInfo->device_id);
            printf("(DEBUG) (%s): CHIP ID:      0x%02x\r\n", pcTaskGetName(NULL), devInfo->chip_id);
            printf("(DEBUG) (%s): FIRMWARE ID:  0x%02x\r\n", pcTaskGetName(NULL), devInfo->firmware_id);
            printf("(DEBUG) (%s): RELEASE CODE: 0x%02x\r\n", pcTaskGetName(NULL), devInfo->release_code);
        }
    }

    return ret;
}

void LVGL_TouchRead(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    GuiBacklightState_t state;
    uint8_t timeout = GUI_TIMEOUT_DEFAULT;
    /* Read coordinates only if display is touched */
    if (FT6236_IsTouched()) {
        if (GUI_BacklightGetState(&state)) {
            GUI_GetTimeout(&timeout);
            GUI_DisplayTimerRestart(timeout*1000);
            // if (GUI_GetTimeout(&timeout)) {
            //     GUI_DisplayTimerRestart(timeout*1000);
            // }
            // else {
            //     GUI_DisplayTimerRestart(GUI_TIMEOUT_DEFAULT*1000);
            // }

            if (state == GUI_BACKLIGHT_OFF) {
                GUI_BacklightOn();
                sl_sleeptimer_delay_millisecond(100);   // delay time for better user experience
            }
            else {
                /* Display is touched, read coordinates */
                uint8_t dataBuffer[5] = {0, 0, 0, 0, 0}; // 1 byte status, 2 bytes X, 2 bytes Y

                FT6236Error_t ret = FT6236_Read(FT6236_TD_STAT_REG, &dataBuffer[0], 5);
                if (ret != FT6236_NO_ERROR) {
                    printf("(ERROR) (%s): Error talking to touch IC: %d \r\n", pcTaskGetName(NULL), ret);
                }

                uint8_t touchPntCnt = dataBuffer[0]; // Number of detected touch points

                if (ret != FT6236_NO_ERROR || touchPntCnt != 1) { // Ignore no touch & multi touch
                    if ( lvgl_touchInputs.currentState != LV_INDEV_STATE_REL) {
                        lvgl_touchInputs.currentState = LV_INDEV_STATE_REL;
                    }
                    data->point.x = lvgl_touchInputs.lastX;
                    data->point.y = lvgl_touchInputs.lastY;
                    data->state = lvgl_touchInputs.currentState;
                    return;
                }

                lvgl_touchInputs.currentState = LV_INDEV_STATE_PR;
                lvgl_touchInputs.lastX = ((dataBuffer[1] & FT6236_MSB_MASK) << 8) | (dataBuffer[2] & FT6236_LSB_MASK);
                lvgl_touchInputs.lastY = ((dataBuffer[3] & FT6236_MSB_MASK) << 8) | (dataBuffer[4] & FT6236_LSB_MASK);

#if LVGL_TOUCH_CONFIG_SWAPXY
                int16_t swapBuffer = lvgl_touchInputs.lastX;
                lvgl_touchInputs.lastX = lvgl_touchInputs.lastY;
                lvgl_touchInputs.lastY = swapBuffer;
#endif
#if LVGL_TOUCH_CONFIG_INVERT_X
                lvgl_touchInputs.lastX =  LV_HOR_RES - lvgl_touchInputs.lastX;
#endif
#if LVGL_TOUCH_CONFIG_INVERT_Y
                lvgl_touchInputs.lastY = LV_VER_RES - lvgl_touchInputs.lastY;
#endif

#if FT6236_CALIBRATION_REQUIRED == 1
                if (lvgl_touchInputs.lastY >= LVGL_TOUCH_CALIB_REGION) {
                    lvgl_touchInputs.lastX = lvgl_touchInputs.lastX - LVGL_TOUCH_X_OFFSET;
                    lvgl_touchInputs.lastY = lvgl_touchInputs.lastY - LVGL_TOUCH_Y_OFFSET;
                }
#endif

                data->point.x = lvgl_touchInputs.lastX;
                data->point.y = lvgl_touchInputs.lastY;

                data->state = lvgl_touchInputs.currentState;
                printf("(DEBUG) (%s): X = %u Y = %u\r\n", pcTaskGetName(NULL), data->point.x, data->point.y);
            }
        }
    }
}
