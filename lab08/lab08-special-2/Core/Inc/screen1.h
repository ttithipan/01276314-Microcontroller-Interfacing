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
  *
  * Touch polling and routing live in the application, see touch.h.  Screen 1
  * only offers its hit tests, so that a supervisor (app.c in
  * lab08-special-2) can decide what a tap means while Screen 2 is on the glass.
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
  * @brief  Refresh the AM2320 readings shown at the top of Screen 1.
  * @note   Call repeatedly from the main loop.  Redraws only when a reading
  *         actually changed, and only when Screen 1 is the visible screen.
  */
void Screen1_Update(void);

/**
  * @brief  The colour produced by the current red/green/blue mixture.
  * @note   Screen 2 uses this as its text colour, per SP2.1.
  */
uint16_t Screen1_MixedColour(void);

/**
  * @brief  Was the large mixed colour circle at the top of Screen 1 tapped?
  * @note   Used by SP2.2 to switch to Screen 2.
  */
uint8_t Screen1_HitMixedCircle(int32_t x, int32_t y);

/**
  * @brief  Apply a tap to the colour controls of Screen 1.
  * @retval 1 when the tap hit one of the red/green/blue circles
  */
uint8_t Screen1_HandleTouch(int32_t x, int32_t y);

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN1_H__ */
