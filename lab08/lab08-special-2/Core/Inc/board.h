/**
  ******************************************************************************
  * @file    board.h
  * @brief   GPIO bring-up for the LCD and touch panel.
  ******************************************************************************
  *
  * The display control lines (PC8/PC9/PC10) and the whole bit banged touch
  * interface (PE2..PE6) are not part of lab08-1.ioc, so CubeMX does not
  * generate them anywhere.  Board_Panel_GPIO_Init() configures them here
  * instead, which also keeps the pins out of gpio.c where a CubeMX
  * regeneration would silently drop them.
  *
  * Pin assignment follows lab08/SCREEN_PINOUT.md:
  *
  *   LCD CS    PC8   CN8-2
  *   LCD DC    PC9   CN8-4
  *   LCD RST   PC10  CN8-6
  *   T_IRQ     PE2   CN9-14
  *   T_CLK     PE3   CN9-22
  *   T_MISO    PE4   CN9-16
  *   T_MOSI    PE5   CN9-18
  *   T_CS      PE6   CN9-20
  *
  * The LCD data lines PF7/PF9 are handled by HAL_SPI_MspInit() in spi.c.
  ******************************************************************************
  */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
  * @brief  Configure the LCD control pins and the touch panel pins.
  * @note   Call once, before ILI9341_Init(), and after HAL_Init().
  */
void Board_Panel_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
