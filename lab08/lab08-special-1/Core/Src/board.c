/**
  ******************************************************************************
  * @file    board.c
  * @brief   GPIO bring-up for the LCD and touch panel.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void Board_Panel_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Port clocks.  MX_GPIO_Init() only enables the ports used by the pins
   * declared in the .ioc file, which do not include GPIOC(panel), GPIOE(touch)
   * and GPIOF(SPI5).  Enabling a clock twice is harmless. */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /* Idle levels are written before the mode is switched to output, so the pins
   * go straight to their idle state without an intermediate glitch:
   * CS high (deselected), DC high (data), RST high (out of reset). */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET);

  /* LCD control lines, push-pull outputs */
  GPIO_InitStruct.Pin = CS_Pin|DC_Pin|RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* Touch clock/data/chip select, push-pull outputs */
  GPIO_InitStruct.Pin = T_CLK_Pin|T_MOSI_Pin|T_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* Touch pen-down IRQ (low while pressed) and touch data-in.
   * Pull resistors stay disabled, as in the reference project: the module is
   * expected to hold T_IRQ high when idle.  If the panel reports a permanent
   * press, add a 10k pull-up on T_IRQ to 3.3 V or set GPIO_PULLUP here. */
  GPIO_InitStruct.Pin = T_IRQ_Pin|T_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
