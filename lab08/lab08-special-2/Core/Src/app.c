/**
  ******************************************************************************
  * @file    app.c
  * @brief   Lab 8, SP2.2 - switching between Screen 1 and Screen 2 by touch.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app.h"

#include "backlight.h"
#include "screen1.h"
#include "screen2.h"
#include "touch.h"

/* USER CODE BEGIN 0 */

volatile App_Screen app_screen = APP_SCREEN_1;

static uint32_t s_screen2_shown_ms = 0U;

/** @brief Put Screen 1 back on the glass.  Screen 2 overwrote all of it. */
static void show_screen_1(void)
{
  Screen1_Init();
  app_screen = APP_SCREEN_1;
}

/** @brief Show Screen 2 in the colour mixed on Screen 1, and start its timer. */
static void show_screen_2(void)
{
  Screen2_Show(Screen1_MixedColour());
  s_screen2_shown_ms = HAL_GetTick();
  app_screen = APP_SCREEN_2;
}

/* USER CODE END 0 */

void App_Init(void)
{
  show_screen_1();
  Backlight_Init();
}

void App_Update(void)
{
  int32_t x = 0;
  int32_t y = 0;

  if (Touch_Poll(&x, &y))
  {
    if (app_screen == APP_SCREEN_1)
    {
      if (Screen1_HitMixedCircle(x, y))
      {
        show_screen_2();
      }
      else
      {
        (void)Screen1_HandleTouch(x, y);
      }
    }
    else if (Screen2_HitPhoto(x, y))
    {
      show_screen_1(); /* leave early instead of waiting out the dwell time */
    }
  }

  if (app_screen == APP_SCREEN_1)
  {
    Screen1_Update(); /* keep the AM2320 readings fresh */
  }
  else if ((HAL_GetTick() - s_screen2_shown_ms) >= APP_SCREEN2_DWELL_MS)
  {
    show_screen_1();
  }

  Backlight_Update();
}
