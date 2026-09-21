/**
  ******************************************************************************
  * @file    am2320.c
  * @brief   Minimal AM2320 temperature / relative humidity sensor driver.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "am2320.h"
#include "i2c.h"

#include <stddef.h>

/* USER CODE BEGIN 0 */

/* Reply frame: 0x03, 0x04, humidity MSB, humidity LSB, temperature MSB,
   temperature LSB, CRC LSB, CRC MSB */
#define AM2320_REPLY_LENGTH      8U
#define AM2320_READ_REGISTERS    0x03U
#define AM2320_START_REGISTER    0x00U
#define AM2320_REGISTER_COUNT    0x04U

/* Wake-up window from the datasheet: at least 0.8 ms, at most 3 ms. */
#define AM2320_WAKE_DELAY_MS     2U

/* The sensor needs about 1.5 ms before it sends the reply frame. */
#define AM2320_REPLY_DELAY_MS    2U

/**
  * @brief  CRC-16/MODBUS, polynomial 0xA001, initial value 0xFFFF.
  * @param  buffer bytes to checksum
  * @param  length number of bytes
  * @retval the 16 bit checksum
  */
static uint16_t AM2320_CRC16(const uint8_t *buffer, uint8_t length)
{
  uint16_t crc = 0xFFFFU;

  for (uint8_t byte = 0; byte < length; byte++)
  {
    crc ^= buffer[byte];
    for (uint8_t bit = 0; bit < 8U; bit++)
    {
      if ((crc & 0x0001U) != 0U)
      {
        crc = (uint16_t)((crc >> 1) ^ 0xA001U);
      }
      else
      {
        crc >>= 1;
      }
    }
  }

  return crc;
}

/**
  * @brief  Restart the I2C peripheral and its pins.
  * @note   HAL_I2C_DeInit() disables the peripheral, resets the handle state
  *         (including the lock and the sticky ErrorCode) and drops PF0/PF1 back
  *         to their reset state; MX_I2C2_Init() then brings both back up.
  */
static void AM2320_I2C_Restart(void)
{
  (void)HAL_I2C_DeInit(&hi2c2);
  MX_I2C2_Init();
}

/**
  * @brief  Check whether the peripheral can start a fresh transfer.
  * @retval 1 when the peripheral is mid-transfer or the handle is not ready
  *
  * The AM2320 is the only device on this bus, and it is not always there: if it
  * is missing, unpowered or miswired, the wake-up transaction never completes
  * and the peripheral can be left inside that failed transfer, holding SCL low
  * with BUSY set.  The HAL then returns HAL_BUSY for every later call, so the
  * reading would stay at "--.-" forever even after the wiring is fixed.
  */
static uint8_t AM2320_I2C_IsStuck(void)
{
  if (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY) != RESET)
  {
    return 1U;
  }

  return (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY) ? 1U : 0U;
}

uint8_t AM2320_Read(AM2320_Data *data)
{
  uint8_t wake = 0x00U;
  uint8_t command[3] = {AM2320_READ_REGISTERS, AM2320_START_REGISTER, AM2320_REGISTER_COUNT};
  uint8_t reply[AM2320_REPLY_LENGTH];

  if (data == NULL)
  {
    return 0U;
  }

  /* Start from a peripheral that is known to be idle, whatever the previous
   * attempt left behind. */
  if (AM2320_I2C_IsStuck())
  {
    AM2320_I2C_Restart();
  }

  /* 1. Wake the sensor up.  A sleeping AM2320 does not acknowledge this
   *    transaction, so an error status here is expected and ignored. */
  (void)HAL_I2C_Master_Transmit(&hi2c2, AM2320_I2C_ADDRESS, &wake, 1, AM2320_I2C_TIMEOUT);
  HAL_Delay(AM2320_WAKE_DELAY_MS);

  /* A failed wake-up pulse can leave the peripheral busy; clear it before the
   * transfer that actually has to succeed. */
  if (AM2320_I2C_IsStuck())
  {
    AM2320_I2C_Restart();
  }

  /* 2. Ask for 4 registers starting at 0x00 (humidity + temperature). */
  if (HAL_I2C_Master_Transmit(&hi2c2, AM2320_I2C_ADDRESS, command, sizeof(command),
                              AM2320_I2C_TIMEOUT) != HAL_OK)
  {
    AM2320_I2C_Restart();
    return 0U;
  }

  HAL_Delay(AM2320_REPLY_DELAY_MS);

  /* 3. Collect the reply frame.  Note the repeated start condition the
   *    sensor expects is what HAL_I2C_Master_Receive generates. */
  if (HAL_I2C_Master_Receive(&hi2c2, AM2320_I2C_ADDRESS | 0x01U, reply, AM2320_REPLY_LENGTH,
                             AM2320_I2C_TIMEOUT) != HAL_OK)
  {
    AM2320_I2C_Restart();
    return 0U;
  }

  /* 4. Frame sanity check before trusting the payload. */
  if ((reply[0] != AM2320_READ_REGISTERS) || (reply[1] != AM2320_REGISTER_COUNT))
  {
    AM2320_I2C_Restart();
    return 0U;
  }

  /* CRC is transmitted low byte first. */
  uint16_t crc_received = (uint16_t)((((uint16_t)reply[7]) << 8) | (uint16_t)reply[6]);
  if (AM2320_CRC16(reply, 6U) != crc_received)
  {
    AM2320_I2C_Restart();
    return 0U;
  }

  /* 5. Decode.  Both values are tens: 0x0115 means 27.7 degrees.
   *    Bit 15 of the temperature word is the sign flag. */
  int16_t temperature = (int16_t)((((uint16_t)reply[4]) << 8) | (uint16_t)reply[5]);
  if ((temperature & 0x8000) != 0)
  {
    temperature = (int16_t)(-(temperature & 0x7FFF));
  }

  uint16_t humidity = (uint16_t)((((uint16_t)reply[2]) << 8) | (uint16_t)reply[3]);

  data->temperature_c = (float)temperature / 10.0f;
  data->humidity_rh   = (float)humidity / 10.0f;

  return 1U;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
