/**
  ******************************************************************************
  * @file    screen2.h
  * @brief   Lab 8, SP2.1 - Screen 2: group, student details and photo.
  ******************************************************************************
  *
  * Draws the layout of Fig 1.2 on the ILI9341 panel in landscape:
  *
  *   +----------------------------------------------------------------+
  *   |  +---------------+   Group No.XX                               |
  *   |  |               |   First name                               |
  *   |  |     photo     |   Last name                                |
  *   |  |               |   Student ID                               |
  *   |  +---------------+                                             |
  *   +----------------------------------------------------------------+
  *
  * White background and a thin black frame, exactly like Screen 1, but the
  * text and the photo outline use the colour mixed on Screen 1 (SP2.1), which
  * is passed in by the caller.
  ******************************************************************************
  */

#ifndef __SCREEN2_H__
#define __SCREEN2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
  * @brief  Draw all of Screen 2.
  * @param  text_colour RGB565 colour for the four text lines and the photo
  *         outline, normally Screen1_MixedColour()
  * @note   ILI9341_Init() and Board_Panel_GPIO_Init() must have run first.
  */
void Screen2_Show(uint16_t text_colour);

/**
  * @brief  Was the photo at the left of Screen 2 tapped?
  * @note   SP2.2 uses this to return to Screen 1 without waiting out the
  *         five second timeout.
  */
uint8_t Screen2_HitPhoto(int32_t x, int32_t y);

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN2_H__ */
