/**
  ******************************************************************************
  * @file    am2320.h
  * @brief   Minimal AM2320 temperature / relative humidity sensor driver.
  ******************************************************************************
  *
  * The AM2320 speaks Modbus-RTU-ish frames over I2C at 7 bit address 0x5C.
  * Two quirks make it different from an ordinary I2C sensor:
  *
  *   1. The device sleeps between conversions to keep its own heating from
  *      skewing the humidity reading, so the first transaction after a sleep
  *      is not acknowledged.  A dummy write acts as a wake-up pulse; the
  *      sensor answers normally from the next transaction on.
  *   2. It needs a pause of about 1.5 ms between the "read registers" command
  *      and the 8 byte reply, which is longer than any normal I2C device.
  *
  * I2C2 (PF0 = SDA, PF1 = SCL) must be initialised with MX_I2C2_Init() before
  * calling AM2320_Read().
  ******************************************************************************
  */

#ifndef __AM2320_H__
#define __AM2320_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 7 bit address 0x5C shifted left, which is the 8 bit form the HAL expects */
#define AM2320_I2C_ADDRESS   (0x5C << 1)
#define AM2320_I2C_TIMEOUT   100U

typedef struct
{
  float temperature_c;  /* -40.0 .. +80.0, resolution 0.1 */
  float humidity_rh;    /*   0.0 .. 100.0, resolution 0.1 */
} AM2320_Data;

/**
  * @brief  Read one temperature/humidity sample.
  * @param  data pointer to the structure that receives the measurement
  * @retval 1 on success, 0 when the sensor did not answer, the reply was
  *         malformed, or the CRC did not match
  */
uint8_t AM2320_Read(AM2320_Data *data);

#ifdef __cplusplus
}
#endif

#endif /* __AM2320_H__ */
