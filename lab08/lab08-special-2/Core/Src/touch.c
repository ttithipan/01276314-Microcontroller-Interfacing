/**
  ******************************************************************************
  * @file    touch.c
  * @brief   Touch panel polling, shared by Screen 1 and Screen 2.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "touch.h"

#include "ILI9341_Touchscreen.h"

/* USER CODE BEGIN 0 */

/* Panel geometry of the rotation the screens are drawn in */
#define SCR_WIDTH               320U
#define SCR_HEIGHT              240U

/*
 * Touch frame rotation.
 *
 * ILI9341_Touchscreen has hardcoded calibration for SCREEN_VERTICAL_1, so its
 * output is expressed in that portrait frame (0..239 across, 0..319 down).
 * The screens are drawn in SCREEN_HORIZONTAL_1, which is the same piece of
 * glass rotated by 90 degrees, so the axes have to be exchanged and possibly
 * mirrored.  Which combination of flips is correct depends on how the panel is
 * mounted, which cannot be decided from source code alone:
 *
 *   press the four corners and watch touch_screen_x / touch_screen_y in the
 *   debugger.  The corner that should read (0, 0) tells you which of the two
 *   TOUCH_FLIP_* switches to change.  The values below are the usual case for
 *   this driver plus SCREEN_HORIZONTAL_1.
 */
#define TOUCH_SWAP_XY           1
#define TOUCH_FLIP_X            0
#define TOUCH_FLIP_Y            1

/* Touch diagnostics, see touch.h */
volatile uint16_t touch_library_x = 0U;
volatile uint16_t touch_library_y = 0U;
volatile int32_t  touch_screen_x  = -1;
volatile int32_t  touch_screen_y  = -1;

/**
  * @brief  Read one touch position and rotate it into the landscape frame.
  * @retval 1 when a usable position was obtained
  */
static uint8_t read_position(int32_t *x, int32_t *y)
{
  uint16_t library[2] = { 0U, 0U };
  int32_t  px;
  int32_t  py;

  if (TP_Read_Coordinates(library) != TOUCHPAD_DATA_OK)
  {
    return 0U;
  }

  touch_library_x = library[0];
  touch_library_y = library[1];

#if TOUCH_SWAP_XY
  px = (int32_t)touch_library_y;
  py = (int32_t)touch_library_x;
#else
  px = (int32_t)touch_library_x;
  py = (int32_t)touch_library_y;
#endif

#if TOUCH_FLIP_X
  px = ((int32_t)SCR_WIDTH - 1) - px;
#endif
#if TOUCH_FLIP_Y
  py = ((int32_t)SCR_HEIGHT - 1) - py;
#endif

  if ((px < 0) || (py < 0) || (px >= (int32_t)SCR_WIDTH) || (py >= (int32_t)SCR_HEIGHT))
  {
    return 0U;
  }

  touch_screen_x = px;
  touch_screen_y = py;

  *x = px;
  *y = py;

  return 1U;
}

uint8_t Touch_Poll(int32_t *x, int32_t *y)
{
  static uint8_t press_handled = 0U;

  if (TP_Touchpad_Pressed() != TOUCHPAD_PRESSED)
  {
    press_handled = 0U;
    return 0U;
  }

  if (press_handled != 0U)
  {
    return 0U;
  }

  /* One action per press.  The read itself is slow and can come back noisy, so
   * a failed attempt is simply retried while the finger stays down. */
  if (read_position(x, y) == 0U)
  {
    return 0U;
  }

  press_handled = 1U;
  return 1U;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
