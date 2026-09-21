/**
  ******************************************************************************
  * @file    backlight.c
  * @brief   Lab 8, SP2.2 - LCD backlight controlled by the potentiometer.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "backlight.h"

#include "adc.h"
#include "tim.h"

/* USER CODE BEGIN 0 */

/* 12 bit ADC, so the raw reading runs 0 .. 4095 */
#define ADC_FULL_SCALE          4095U

/* The PWM runs at 1000 counts per period, so 1 % is 10 counts */
#define PWM_COUNTS_PER_PERCENT  10U

/* Averaging a few readings keeps ADC noise from making the backlight flicker */
#define ADC_SAMPLES             4U

/* Re-reading the potentiometer at 10 Hz is plenty for a human turning a knob */
#define UPDATE_PERIOD_MS        100U

volatile uint8_t backlight_percent = BACKLIGHT_MAX_PERCENT;

static uint32_t s_last_update_ms = 0U;

/** @brief One averaged ADC reading from the backlight potentiometer. */
static uint32_t read_potentiometer(void)
{
  uint32_t total = 0U;
  uint32_t taken = 0U;

  for (uint32_t sample = 0U; sample < ADC_SAMPLES; sample++)
  {
    if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
    {
      break;
    }
    total += HAL_ADC_GetValue(&hadc1);
    taken++;
  }

  if (taken == 0U)
  {
    return ADC_FULL_SCALE; /* ADC not answering: keep the screen bright */
  }

  return total / taken;
}

void Backlight_Init(void)
{
  /* ADC1 runs in continuous conversion mode, so kicking it off once is enough
   * and each later poll returns a fresh sample. */
  (void)HAL_ADC_Start(&hadc1);

  (void)HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

  s_last_update_ms = 0U; /* force the first Backlight_Update() to run */
}

void Backlight_Update(void)
{
  uint32_t now = HAL_GetTick();
  uint32_t raw;
  uint8_t  percent;

  if ((s_last_update_ms != 0U) && ((now - s_last_update_ms) < UPDATE_PERIOD_MS))
  {
    return;
  }
  s_last_update_ms = now;

  raw = read_potentiometer();

  /* Scale 0..4095 onto 20..100 % */
  percent = (uint8_t)(BACKLIGHT_MIN_PERCENT
                      + ((raw * (BACKLIGHT_MAX_PERCENT - BACKLIGHT_MIN_PERCENT)) / ADC_FULL_SCALE));

  if (percent == backlight_percent)
  {
    return; /* nothing to redraw, and no audible PWM whine from reloading */
  }

  backlight_percent = percent;
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (uint32_t)percent * PWM_COUNTS_PER_PERCENT);
}
