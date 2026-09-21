/**
  ******************************************************************************
  * @file    screen1.h
  * @brief   Lab 8, Experiment 1 - Screen 1: mixing RGB colour on the LCD.
  ******************************************************************************
  *
  * Draws the layout of Fig 1.1 on the ILI9341 panel in landscape:
  *
  *   +----------------------------------------------------------------+
  *   | 27.1 C              (mixed colour)              55.6 %RH       |
  *   |                       circle                                   |
  *   |  (red)   [##########.....................]      80 %           |
  *   |  (green) [####...........................]      40 %           |
  *   |  (blue)  [#######........................]      70 %           |
  *   +----------------------------------------------------------------+
  *
  * Touching one of the three colour circles raises that channel by 10 %,
  * wrapping back to 0 % after 100 % so the value can be cycled.
  ******************************************************************************
  */

#ifndef __SCREEN1_H__
#define __SCREEN1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
  * @brief  Set the landscape rotation, clear the panel and draw all of Screen 1.
  * @note   ILI9341_Init() and Board_Panel_GPIO_Init() must have run first.
  */
void Screen1_Init(void);

/**
  * @brief  Service Screen 1: poll the touch panel and refresh the sensor.
  * @note   Call repeatedly from the main loop.  Blocking, no interrupts used.
  */
void Screen1_Update(void);

/**
  * Touch diagnostics, useful while bringing the panel up: watch them in the
  * debugger while pressing the four corners of the screen.
  *
  *   touch_library_x/y : what ILI9341_Touchscreen returns (its own hardcoded
  *                       SCREEN_VERTICAL_1 frame, 0..239 by 0..319)
  *   touch_screen_x/y  : the same point after the landscape remap, i.e. the
  *                       coordinates actually used for hit testing
  */
extern volatile uint16_t touch_library_x;
extern volatile uint16_t touch_library_y;
extern volatile int32_t  touch_screen_x;
extern volatile int32_t  touch_screen_y;

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN1_H__ */
