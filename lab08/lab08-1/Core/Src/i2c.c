/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/*
 * I2C2 for the AM2320 temperature/humidity sensor.
 *
 * Wiring per lab08/AM2320_PINOUT.md:
 *   PF0  CN9-21  I2C2_SDA
 *   PF1  CN9-19  I2C2_SCL
 * Both are unused by the LCD (SPI5 on PF7/PF8/PF9) and by the touch panel
 * (PE2..PE6).
 *
 * Timing.  I2C only pulls lines low; something has to pull them back up, and
 * the rise time is R(pull-up) x C(bus).  The resistors in AM2320_PINOUT.md are
 * 4.7 kOhm, which is what the 100 kHz setting below assumes.
 *
 * If the bus is run on the STM32's internal pull-ups alone (~40 kOhm) the edges
 * are several microseconds instead of the ~1 us that 100 kHz allows, the
 * peripheral misreads its own traffic and reports BERR, and the sensor is never
 * reached.  Running the same weak pull-ups at 10 kHz leaves roughly 50 us for
 * each edge to settle, which in practice makes a pull-up-less bus usable.  Both
 * values are 7 bit, 100 kHz-capable AM2320 settings; the slow one is only slower.
 */
#define AM2320_I2C_TIMING_100KHZ  0xB0401214U  /* 40k pull-ups: external 4k7 fitted   */
#define AM2320_I2C_TIMING_10KHZ   0xF0309595U  /* 40k pull-ups: internal pull-ups only */

/* Select the timing that matches the hardware: external resistors, or the
 * internal pull-ups driving a bus with no resistors at all. */
#define AM2320_I2C_TIMING         AM2320_I2C_TIMING_10KHZ

void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = AM2320_I2C_TIMING;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PF0     ------> I2C2_SDA  (CN9-21)
    PF1     ------> I2C2_SCL  (CN9-19)
    */
    GPIO_InitStruct.Pin = AM2320_I2C_SDA_Pin|AM2320_I2C_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(AM2320_I2C_SDA_GPIO_Port, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PF0     ------> I2C2_SDA
    PF1     ------> I2C2_SCL
    */
    HAL_GPIO_DeInit(AM2320_I2C_SDA_GPIO_Port, AM2320_I2C_SDA_Pin|AM2320_I2C_SCL_Pin);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
