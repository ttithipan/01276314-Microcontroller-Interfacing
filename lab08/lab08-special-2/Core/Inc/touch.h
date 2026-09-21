/**
  ******************************************************************************
  * @file    touch.h
  * @brief   Touch panel polling, shared by Screen 1 and Screen 2.
  ******************************************************************************
  *
  * ILI9341_Touchscreen bit-bangs its own SPI and returns coordinates in the
  * frame it was calibrated for, SCREEN_VERTICAL_1.  This module hides both the
  * rotation into the landscape frame the screens are drawn in, and the
  * one-action-per-press rule, so the screens only deal with screen pixels.
  ******************************************************************************
  */

#ifndef __TOUCH_H__
#define __TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
  * @brief  Get one touch position, at most once per press.
  * @param  x receives the screen X coordinate when 1 is returned
  * @param  y receives the screen Y coordinate when 1 is returned
  * @retval 1 for a new, usable press; 0 when idle, already handled, or noisy
  * @note   A press that fails to decode is retried on the next call, so a
  *         short tap is not lost.  The next press is only reported after the
  *         panel has been released.
  */
uint8_t Touch_Poll(int32_t *x, int32_t *y);

/**
  * Touch diagnostics, useful while bringing a panel up: watch them in the
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

#endif /* __TOUCH_H__ */
