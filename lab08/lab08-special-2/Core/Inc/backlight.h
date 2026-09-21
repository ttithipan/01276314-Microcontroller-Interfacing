/**
  ******************************************************************************
  * @file    backlight.h
  * @brief   Lab 8, SP2.2 - LCD backlight controlled by the potentiometer.
  ******************************************************************************
  *
  * The potentiometer on PC0 (ADC1_IN10) is mapped onto the backlight PWM on
  * PB10 (TIM2_CH3) over the range the handout asks for, 20 % to 100 %:
  *
  *     ADC 0      ->  20 % duty   (pot at one end, backlight as dim as allowed)
  *     ADC 4095   -> 100 % duty   (pot at the other end, backlight fully on)
  *
  * The range is deliberately never 0 %: the screen must stay readable, and the
  * handout specifies a 20 % floor.
  ******************************************************************************
  */

#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/** Duty cycle range required by SP2.2, in percent */
#define BACKLIGHT_MIN_PERCENT   20U
#define BACKLIGHT_MAX_PERCENT   100U

/**
  * @brief  Start the PWM and apply the first duty cycle.
  * @note   MX_TIM2_Init() and MX_ADC1_Init() must have run first.
  */
void Backlight_Init(void);

/**
  * @brief  Follow the potentiometer.
  * @note   Call repeatedly from the main loop; it re-reads and re-programs
  *         only a few times a second, and only when the value really changed.
  */
void Backlight_Update(void);

/**
  * @brief  The duty cycle currently programmed, in percent (20..100).
  * @note   Handy in the debugger while checking the potentiometer wiring.
  */
extern volatile uint8_t backlight_percent;

#ifdef __cplusplus
}
#endif

#endif /* __BACKLIGHT_H__ */
