/**
  ******************************************************************************
  * @file    screen1.c
  * @brief   Lab 8, Experiment 1 - Screen 1: mixing RGB colour on the LCD.
  ******************************************************************************
  *
  * Layout, colours and font sizes follow Fig 1.1 of the Lab 8 handout.  Only
  * the parts that actually change are repainted: the ILI9341 driver pushes
  * individual pixels over SPI one at a time, so a full screen repaint (text
  * included) would take hundreds of milliseconds and visibly flicker.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "screen1.h"
#include "am2320.h"

#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

#include <stdio.h>
#include <string.h>

/* USER CODE BEGIN 0 */

/* Panel geometry, SCREEN_HORIZONTAL_1 */
#define SCR_WIDTH               320U
#define SCR_HEIGHT              240U

#define SCREEN1_BACKGROUND      WHITE
#define SCREEN1_TEXT            BLACK

/* Thin black frame around the whole layout */
#define BORDER_X0               1U
#define BORDER_Y0               1U
#define BORDER_X1               (SCR_WIDTH - 2U)
#define BORDER_Y1               (SCR_HEIGHT - 2U)

/* Glyph cell of Drivers/ILI9341/5x5_font.h: 6 columns wide, 8 rows tall.
 * Keep in sync with CHAR_WIDTH / CHAR_HEIGHT if the font is replaced. */
#define FONT_CELL_W             6U
#define FONT_CELL_H             8U

/* Font sizes (the driver scales the 5x5 font by an integer factor) */
#define TEXT_SIZE_READING       2U
#define TEXT_SIZE_VALUE         3U

/* Top band: sensor readings left and right, mixed colour circle in between */
#define READING_Y               36U
#define TEMP_FIELD_X            8U
#define TEMP_FIELD_CELLS        7U   /* "-40.0 C"  */
#define HUM_FIELD_CELLS         9U   /* "100.0 %RH" */
#define HUM_FIELD_RIGHT         (SCR_WIDTH - 8U)
#define HUM_FIELD_X             (HUM_FIELD_RIGHT - (HUM_FIELD_CELLS * FONT_CELL_W * TEXT_SIZE_READING))

#define MIX_CX                  148U
#define MIX_CY                  44U
#define MIX_R                   30U

/* Radius around the mixed colour circle that still counts as a hit. Used by
 * SP2.2 to decide whether the user asked for Screen 2. */
#define MIX_TOUCH_R             (MIX_R + 12U)

/* The three colour control rows */
#define ROW_FIRST_CY            106U
#define ROW_STEP                48U
#define SWATCH_CX               38U
#define SWATCH_R                20U
#define BAR_X                   70U
#define BAR_W                   146U
#define BAR_H                   26U

#define PCT_FIELD_CELLS         5U   /* "100 %" */
#define PCT_FIELD_RIGHT         (SCR_WIDTH - 8U)

/* Radius around each colour circle that still counts as a hit. Larger than the
 * drawn circle so the user does not have to aim exactly. */
#define TOUCH_TARGET_R          30

/* A fresh sensor sample every second: the AM2320 refreshes its internal
 * measurement about every two seconds, and a read costs roughly 8 ms. */
#define SENSOR_PERIOD_MS        1000U

/* Room for the longest reading text plus its terminator.  Generous on
 * purpose: the readings themselves are bounded, but the compiler cannot know
 * that when it checks the snprintf format. */
#define READING_TEXT_MAX        24U

/* Fraction of the pure colour kept when drawing the unfilled part of a bar:
 * 25 = a pale tint, i.e. 25 % colour mixed into 75 % white. */
#define TINT_KEEP_PERCENT       25U

/* Channels, in the order the rows are drawn */
#define CHANNEL_RED             0U
#define CHANNEL_GREEN           1U
#define CHANNEL_BLUE            2U
#define CHANNEL_COUNT           3U

/* RGB levels in percent, initialised to the values shown in Fig 1.1 */
static uint8_t s_level[CHANNEL_COUNT] = { 80U, 40U, 70U };

/* What is currently on the glass, so that Screen1_Update() can skip redraws */
static char s_shown_temperature[READING_TEXT_MAX] = "";
static char s_shown_humidity[READING_TEXT_MAX] = "";

static uint32_t s_last_sensor_ms = 0U;

/* Full saturation colour drawn in the filled part of each bar.  The pale
 * "empty" tint is derived from these by blend_with_white(). */
static const uint16_t CHANNEL_COLOUR[CHANNEL_COUNT] =
{
  RED,
  GREEN,
  BLUE
};

/* USER CODE END 0 */

/* Private helpers ----------------------------------------------------------*/

/**
  * @brief  Mix a colour with white.
  * @param  colour RGB565 source colour
  * @param  keep_percent how much of the source colour survives, 0..100
  */
static uint16_t blend_with_white(uint16_t colour, uint8_t keep_percent)
{
  uint16_t red   = (uint16_t)((colour >> 11) & 0x1FU);
  uint16_t green = (uint16_t)((colour >> 5) & 0x3FU);
  uint16_t blue  = (uint16_t)(colour & 0x1FU);
  uint16_t keep  = (uint16_t)keep_percent;
  uint16_t drop  = (uint16_t)(100U - keep);

  red   = (uint16_t)((red   * keep + 31U * drop) / 100U);
  green = (uint16_t)((green * keep + 63U * drop) / 100U);
  blue  = (uint16_t)((blue  * keep + 31U * drop) / 100U);

  return (uint16_t)((red << 11) | (green << 5) | blue);
}

uint16_t Screen1_MixedColour(void)
{
  uint16_t red   = (uint16_t)((s_level[CHANNEL_RED]   * 31U + 50U) / 100U);
  uint16_t green = (uint16_t)((s_level[CHANNEL_GREEN] * 63U + 50U) / 100U);
  uint16_t blue  = (uint16_t)((s_level[CHANNEL_BLUE]  * 31U + 50U) / 100U);

  return (uint16_t)((red << 11) | (green << 5) | blue);
}

/** @brief Row centre line of one colour control row. */
static uint16_t row_center(uint8_t channel)
{
  return (uint16_t)(ROW_FIRST_CY + ((uint16_t)channel * ROW_STEP));
}

/** @brief Blit a string with its left edge at X. */
static void text_left(const char *text, uint16_t x, uint16_t y, uint16_t size)
{
  ILI9341_Draw_Text(text, x, y, SCREEN1_TEXT, size, SCREEN1_BACKGROUND);
}

/** @brief Blit a string with its right edge at right_x. */
static void text_right(const char *text, uint16_t right_x, uint16_t y, uint16_t size)
{
  uint16_t width = (uint16_t)(strlen(text) * FONT_CELL_W * size);

  if (width >= right_x)
  {
    return;
  }
  ILI9341_Draw_Text(text, (uint16_t)(right_x - width), y, SCREEN1_TEXT, size, SCREEN1_BACKGROUND);
}

/* Drawing ------------------------------------------------------------------*/

/** @brief The three solid colour circles on the left of each row. */
static void draw_swatches(void)
{
  for (uint8_t channel = 0U; channel < CHANNEL_COUNT; channel++)
  {
    ILI9341_Draw_Filled_Circle(SWATCH_CX, row_center(channel), SWATCH_R, CHANNEL_COLOUR[channel]);
  }
}

/**
  * @brief  Draw one brightness bar: saturated colour up to the level, then a
  *         pale tint of the same colour for the rest of the track.
  * @note   Both rectangles together cover the whole track, so the previous
  *         content is overwritten and no erase pass is needed.
  */
static void draw_bar(uint8_t channel)
{
  uint16_t y       = (uint16_t)(row_center(channel) - (BAR_H / 2U));
  uint16_t filled  = (uint16_t)(((uint32_t)BAR_W * s_level[channel]) / 100U);

  if (filled > 0U)
  {
    ILI9341_Draw_Rectangle(BAR_X, y, filled, BAR_H, CHANNEL_COLOUR[channel]);
  }
  if (filled < BAR_W)
  {
    ILI9341_Draw_Rectangle((uint16_t)(BAR_X + filled), y, (uint16_t)(BAR_W - filled), BAR_H,
                           blend_with_white(CHANNEL_COLOUR[channel], TINT_KEEP_PERCENT));
  }
}

/** @brief The percentage readout on the right of one colour row. */
static void draw_percentage(uint8_t channel)
{
  char text[8];
  uint16_t field_width = (uint16_t)(PCT_FIELD_CELLS * FONT_CELL_W * TEXT_SIZE_VALUE);
  uint16_t field_y     = (uint16_t)(row_center(channel) - ((FONT_CELL_H * TEXT_SIZE_VALUE) / 2U));

  /* "%3d %%" gives a fixed five character field, so the digits do not shift
   * sideways as the value grows from 0 % to 100 %. */
  snprintf(text, sizeof(text), "%3d %%", (int)s_level[channel]);

  /* Erase first: a shorter string would otherwise leave digits behind. */
  ILI9341_Draw_Rectangle((uint16_t)(PCT_FIELD_RIGHT - field_width), field_y, field_width,
                         (uint16_t)(FONT_CELL_H * TEXT_SIZE_VALUE), SCREEN1_BACKGROUND);
  text_right(text, PCT_FIELD_RIGHT, field_y, TEXT_SIZE_VALUE);
}

/** @brief The large circle in the top band showing the mixed colour. */
static void draw_mixed_circle(void)
{
  ILI9341_Draw_Filled_Circle(MIX_CX, MIX_CY, MIX_R, Screen1_MixedColour());
}

/** @brief Redraw one sensor field, but only when the text really changed. */
static void draw_reading(const char *text, uint16_t field_x, uint16_t field_cells,
                         uint8_t align_right, char *shown)
{
  uint16_t field_width = (uint16_t)(field_cells * FONT_CELL_W * TEXT_SIZE_READING);

  if (strcmp(text, shown) == 0)
  {
    return;
  }

  /* Clear the whole field first: a shorter string than last time would
   * otherwise leave stale digits behind. */
  ILI9341_Draw_Rectangle(field_x, READING_Y, field_width,
                         (uint16_t)(FONT_CELL_H * TEXT_SIZE_READING), SCREEN1_BACKGROUND);

  if (align_right)
  {
    text_right(text, (uint16_t)(field_x + field_width), READING_Y, TEXT_SIZE_READING);
  }
  else
  {
    text_left(text, field_x, READING_Y, TEXT_SIZE_READING);
  }

  strncpy(shown, text, READING_TEXT_MAX - 1U);
  shown[READING_TEXT_MAX - 1U] = '\0';
}

/** @brief Sample the AM2320 and refresh the two readings at the top. */
static void refresh_readings(void)
{
  AM2320_Data sample;
  char text[READING_TEXT_MAX];

  if (AM2320_Read(&sample))
  {
    /* Format from tenths to keep printf free of floating point support. */
    int32_t tenths = (int32_t)((sample.temperature_c * 10.0f) + ((sample.temperature_c < 0.0f) ? -0.5f : 0.5f));

    if (tenths < 0)
    {
      snprintf(text, sizeof(text), "-%ld.%ld C", (long)((-tenths) / 10), (long)((-tenths) % 10));
    }
    else
    {
      snprintf(text, sizeof(text), "%ld.%ld C", (long)(tenths / 10), (long)(tenths % 10));
    }
    draw_reading(text, TEMP_FIELD_X, TEMP_FIELD_CELLS, 0U, s_shown_temperature);

    tenths = (int32_t)((sample.humidity_rh * 10.0f) + 0.5f);
    snprintf(text, sizeof(text), "%ld.%ld %%RH", (long)(tenths / 10), (long)(tenths % 10));
    draw_reading(text, HUM_FIELD_X, HUM_FIELD_CELLS, 1U, s_shown_humidity);
  }
  else
  {
    /* Sensor missing or wired wrong: say so instead of freezing on a stale
     * number.  Both readings are retried on the next update. */
    draw_reading("--.- C", TEMP_FIELD_X, TEMP_FIELD_CELLS, 0U, s_shown_temperature);
    draw_reading("--.- %RH", HUM_FIELD_X, HUM_FIELD_CELLS, 1U, s_shown_humidity);
  }
}

/* Public interface ---------------------------------------------------------*/

void Screen1_Init(void)
{
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
  ILI9341_Fill_Screen(SCREEN1_BACKGROUND);

  ILI9341_Draw_Hollow_Rectangle_Coord(BORDER_X0, BORDER_Y0, BORDER_X1, BORDER_Y1, SCREEN1_TEXT);

  draw_swatches();

  for (uint8_t channel = 0U; channel < CHANNEL_COUNT; channel++)
  {
    draw_bar(channel);
    draw_percentage(channel);
  }

  draw_mixed_circle();

  /* Force both readings to be redrawn even if they did not change since the
   * previous time Screen 1 was shown. */
  s_shown_temperature[0] = '\0';
  s_shown_humidity[0] = '\0';

  refresh_readings();

  s_last_sensor_ms = HAL_GetTick();
}

void Screen1_Update(void)
{
  uint32_t now = HAL_GetTick();

  if ((now - s_last_sensor_ms) >= SENSOR_PERIOD_MS)
  {
    s_last_sensor_ms = now;
    refresh_readings();
  }
}

uint8_t Screen1_HitMixedCircle(int32_t x, int32_t y)
{
  int32_t dx = x - (int32_t)MIX_CX;
  int32_t dy = y - (int32_t)MIX_CY;

  return (((dx * dx) + (dy * dy)) <= ((int32_t)MIX_TOUCH_R * (int32_t)MIX_TOUCH_R)) ? 1U : 0U;
}

uint8_t Screen1_HandleTouch(int32_t x, int32_t y)
{
  int32_t best_distance = (int32_t)TOUCH_TARGET_R * TOUCH_TARGET_R;
  int32_t best_channel  = -1;

  /* The radius test is made against every row and the nearest circle wins, so
   * the overlap between neighbouring hit areas cannot make the wrong colour
   * respond. */
  for (uint8_t channel = 0U; channel < CHANNEL_COUNT; channel++)
  {
    int32_t dx = x - (int32_t)SWATCH_CX;
    int32_t dy = y - (int32_t)row_center(channel);
    int32_t distance = (dx * dx) + (dy * dy);

    if (distance < best_distance)
    {
      best_distance = distance;
      best_channel  = (int32_t)channel;
    }
  }

  if (best_channel < 0)
  {
    return 0U; /* touched the panel somewhere that is not a colour control */
  }

  /* Step up by 10 %, wrapping 100 % back to 0 % so the value can be cycled. */
  s_level[best_channel] = (s_level[best_channel] >= 100U) ? 0U : (uint8_t)(s_level[best_channel] + 10U);

  draw_bar((uint8_t)best_channel);
  draw_percentage((uint8_t)best_channel);
  draw_mixed_circle();

  return 1U;
}
