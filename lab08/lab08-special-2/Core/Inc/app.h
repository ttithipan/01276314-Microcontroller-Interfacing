/**
  ******************************************************************************
  * @file    app.h
  * @brief   Lab 8, SP2.2 - switching between Screen 1 and Screen 2 by touch.
  ******************************************************************************
  *
  * Behaviour required by SP2.2:
  *
  *   - Screen 1 is shown as soon as the board starts.
  *   - Tapping the large mixed colour circle at the top of Screen 1 shows
  *     Screen 2 for five seconds, then returns to Screen 1 by itself.
  *   - Tapping the photo on Screen 2 returns to Screen 1 immediately.
  *   - The backlight follows the potentiometer between 20 % and 100 % at all
  *     times, independently of which screen is up.
  *
  * The colour controls of Screen 1 stay live while Screen 1 is up, so the
  * colour mixed on Screen 1 is whatever the user has dialled in when Screen 2
  * is shown, which is the colour Screen 2 uses for its text (SP2.1).
  ******************************************************************************
  */

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/** How long Screen 2 stays up on its own, as required by SP2.2 */
#define APP_SCREEN2_DWELL_MS    5000U

/** The screen currently on the glass */
typedef enum
{
  APP_SCREEN_1 = 0,
  APP_SCREEN_2 = 1
} App_Screen;

/**
  * @brief  Draw Screen 1 and start the backlight control.
  * @note   ILI9341_Init() and Board_Panel_GPIO_Init() must have run first.
  */
void App_Init(void);

/**
  * @brief  Service the current screen, the touch panel and the backlight.
  * @note   Call repeatedly from the main loop.
  */
void App_Update(void);

/**
  * @brief  The screen currently displayed, for the debugger.
  */
extern volatile App_Screen app_screen;

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
