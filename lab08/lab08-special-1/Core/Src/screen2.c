/**
  ******************************************************************************
  * @file    screen2.c
  * @brief   Lab 8, SP2.1 - Screen 2: group, student details and photo.
  ******************************************************************************
  *
  * The photo is a 132x176 RGB565 bitmap produced by tools/make_team_photo.py.
  * The two text sizes below use the same integer scaled 6x8 font as Screen 1.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "screen2.h"
#include "team_photo.h"

#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "spi.h"

/* USER CODE BEGIN 0 */

/* ==== EDIT ME =============================================================
 * Screen 2 shows the group number and the three students of the group.  The
 * values below are the placeholders drawn in Fig 1.2; replace them with your
 * own.  Keep them at most 13 characters so they stay inside the frame.
 * ======================================================================== */
#define SCREEN2_GROUP_TEXT      "Group No.09"
#define SCREEN2_FIRST_NAME      "Chuthathip"
#define SCREEN2_LAST_NAME       "Termchaikul"
#define SCREEN2_STUDENT_ID      "67011594"

/* Panel geometry, SCREEN_HORIZONTAL_1 */
#define SCR_WIDTH               320U
#define SCR_HEIGHT              240U

#define SCREEN2_BACKGROUND      WHITE
#define SCREEN2_FRAME           BLACK

/* Thin black frame around the whole layout, matching Screen 1 */
#define BORDER_X0               1U
#define BORDER_Y0               1U
#define BORDER_X1               (SCR_WIDTH - 2U)
#define BORDER_Y1               (SCR_HEIGHT - 2U)

/* Glyph cell of Drivers/ILI9341/5x5_font.h */
#define FONT_CELL_W             6U

/* The four details are drawn at 2x, which keeps the longest line (13 cells,
 * 156 px) inside the frame when it starts at TEXT_X. */
#define TEXT_SIZE               2U
#define TEXT_X                  160U
#define TEXT_Y_FIRST            36U
#define TEXT_Y_STEP             46U

/* Photo area, left column.  Must match the bitmap from make_team_photo.py;
 * the preprocessor check below fails the build if the two ever disagree. */
#define PHOTO_X                 14U
#define PHOTO_Y                 28U
#define PHOTO_W                 132U
#define PHOTO_H                 176U

#if (PHOTO_W != TEAM_PHOTO_WIDTH) || (PHOTO_H != TEAM_PHOTO_HEIGHT)
#error "screen2.c photo area does not match team_photo.h; rerun tools/make_team_photo.py or fix PHOTO_W/PHOTO_H"
#endif

/* Slack around the photo that still counts as touching it, in pixels */
#define PHOTO_TOUCH_SLACK       12

/* Pixels pushed per SPI burst while sending the bitmap.  Keep well below the
 * driver's BURST_MAX_SIZE so the same stack budget applies. */
#define BITMAP_BURST_SIZE       400U

/* USER CODE END 0 */

/* Private helpers ----------------------------------------------------------*/

/**
  * @brief  Send a rectangle of pre-packed big-endian RGB565 pixels.
  * @note   The library's ILI9341_Draw_Image() always covers the whole screen,
  *         so a windowed version is needed for an inset photo.
  */
static void draw_bitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                        const uint8_t *pixels)
{
  uint32_t remaining = (uint32_t)width * (uint32_t)height * 2U;
  const uint8_t *cursor = pixels;

  ILI9341_Set_Address(x, y, (uint16_t)(x + width - 1U), (uint16_t)(y + height - 1U));

  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

  while (remaining > 0U)
  {
    uint16_t chunk = (remaining > BITMAP_BURST_SIZE) ? BITMAP_BURST_SIZE : (uint16_t)remaining;

    (void)HAL_SPI_Transmit(HSPI_INSTANCE, (uint8_t *)(uintptr_t)cursor, chunk, 100U);

    cursor += chunk;
    remaining -= chunk;
  }

  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/** @brief Blit one of the four detail lines. */
static void draw_line(const char *text, uint16_t y, uint16_t colour)
{
  ILI9341_Draw_Text(text, TEXT_X, y, colour, TEXT_SIZE, SCREEN2_BACKGROUND);
}

/* Public interface ---------------------------------------------------------*/

void Screen2_Show(uint16_t text_colour)
{
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
  ILI9341_Fill_Screen(SCREEN2_BACKGROUND);

  ILI9341_Draw_Hollow_Rectangle_Coord(BORDER_X0, BORDER_Y0, BORDER_X1, BORDER_Y1, SCREEN2_FRAME);

  /* One pixel outline around the photo, in the colour mixed on Screen 1 */
  ILI9341_Draw_Hollow_Rectangle_Coord((uint16_t)(PHOTO_X - 1U), (uint16_t)(PHOTO_Y - 1U),
                                      (uint16_t)(PHOTO_X + PHOTO_W), (uint16_t)(PHOTO_Y + PHOTO_H),
                                      text_colour);

  draw_bitmap(PHOTO_X, PHOTO_Y, PHOTO_W, PHOTO_H, team_photo_rgb565);

  draw_line(SCREEN2_GROUP_TEXT, TEXT_Y_FIRST, text_colour);
  draw_line(SCREEN2_FIRST_NAME, (uint16_t)(TEXT_Y_FIRST + TEXT_Y_STEP), text_colour);
  draw_line(SCREEN2_LAST_NAME, (uint16_t)(TEXT_Y_FIRST + (2U * TEXT_Y_STEP)), text_colour);
  draw_line(SCREEN2_STUDENT_ID, (uint16_t)(TEXT_Y_FIRST + (3U * TEXT_Y_STEP)), text_colour);
}

uint8_t Screen2_HitPhoto(int32_t x, int32_t y)
{
  int32_t left   = (int32_t)PHOTO_X - PHOTO_TOUCH_SLACK;
  int32_t top    = (int32_t)PHOTO_Y - PHOTO_TOUCH_SLACK;
  int32_t right  = (int32_t)(PHOTO_X + PHOTO_W) + PHOTO_TOUCH_SLACK;
  int32_t bottom = (int32_t)(PHOTO_Y + PHOTO_H) + PHOTO_TOUCH_SLACK;

  if ((x < left) || (x > right) || (y < top) || (y > bottom))
  {
    return 0U;
  }

  return 1U;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
