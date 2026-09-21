# AM2320 sensor wiring for NUCLEO-F767ZI

Use **I2C2 on PF0 (SDA) and PF1 (SCL)** to read temperature and relative humidity for Screen 1 in [Lab 8](docs/2026-Lab-08-LCD.md). These two GPIOs are unused by the existing LCD and touch configuration. UM1974 Rev 11, Table 19, page 61 confirms their board connector assignments.

## Wiring

| AM2320 pin | Sensor signal | Nucleo connection | MCU pin / function | Board signal |
| --- | --- | --- | --- | --- |
| 1 | VDD / VCC | CN8 pin 7, +3.3 V rail | Power, not a GPIO | +3.3 V |
| 2 | SDA | **CN9 pin 21** | **PF0 / I2C2_SDA** | D68 |
| 3 | GND | **CN9 pin 23** | Common ground | GND |
| 4 | SCL | **CN9 pin 19** | **PF1 / I2C2_SCL** | D69 |

The pin numbers above refer to the bare four-pin AM2320 sensor: with the front grille facing you and the leads pointing downward, pins 1-4 run left to right. If your sensor is on a breakout board, follow its printed VCC/SDA/GND/SCL labels; the header order may differ.

`CN9 pin 21` is a physical board header position; `PF0` is the MCU GPIO name, not a numbered chip-package lead. CN9 pins 19, 21, and 23 are consecutive positions along the odd-numbered row. Identify the connector's pin-1 marking before counting.

## I2C pull-up resistors

For short breadboard wiring, use two **4.7 kΩ** pull-up resistors:

```text
3.3 V ----[4.7 kΩ]---- SDA ---- CN9-21 / PF0
3.3 V ----[4.7 kΩ]---- SCL ---- CN9-19 / PF1
```

Each resistor connects its signal line to 3.3 V; neither goes in series with the signal wire. The bare AM2320 needs external pull-ups. If a breakout already has suitable pull-ups to 3.3 V, use those instead of automatically adding another pair. Powering the sensor and its pull-ups from 3.3 V keeps the bus at the Nucleo's logic voltage. See the [AM2320 pinout guide](https://learn.adafruit.com/adafruit-am2320-temperature-humidity-i2c-sensor/pinouts) and [wiring guide](https://learn.adafruit.com/adafruit-am2320-temperature-humidity-i2c-sensor/arduino-usage).

## Screen conflict check

| Device | GPIO pins reserved |
| --- | --- |
| LCD SPI5 | PF7, PF8, PF9 |
| LCD control | PC8, PC9, PC10 |
| Touch interface | PE2, PE3, PE4, PE5, PE6 |
| AM2320 I2C2 | **PF0, PF1** |

**No sensor signal pin overlaps a screen or touch pin.** PF0 and PF1 are also unassigned in the supplied `767_SPI_ILI9341_2.ioc` file. Leave PF2 / CN9-17 unused; the AM2320 does not need the I2C SMBus-alert signal.

The sensor and screen should share the same ground and 3.3 V supply rail. This is intentional power sharing, not a GPIO conflict. If CN8-7 already powers the screen or backlight, distribute its 3.3 V connection through a breadboard power rail; do not replace the screen connection. CN9-23 gives the sensor a separate ground header position.

The screen's terminal labelled SDI/MOSI or SDA is an SPI data input connected to PF9. Do not join it to the AM2320's I2C SDA line.

## Firmware configuration for the later implementation

- Enable **I2C2** and select **PF0 = I2C2_SDA**, **PF1 = I2C2_SCL** in CubeMX.
- Use alternate-function open-drain GPIOs (AF4), with the external pull-ups above; start with a 100 kHz I2C bus.
- The AM2320 has fixed 7-bit address **0x5C**. STM32 HAL I2C calls using the shifted address convention require `0x5C << 1` (`0xB8`).
- Read both temperature and relative humidity and display them in the top-left and top-right positions respectively, as in Fig 1.1. The figure's `27.1 C` and `55.6 %RH` are examples, not fixed readings.

This file specifies wiring only. I2C2 initialization, the AM2320 driver, and LCD data updates have not been added or hardware-tested.

## Sources

- [UM1974 board manual](docs/um1974-stm32-nucleo144-boards-mb1137-stmicroelectronics.pdf), Rev 11, Table 19, page 61: PF0/PF1, CN9-19/21/23, and CN8-7.
- [Screen pin list](SCREEN_PINOUT.md), [project configuration](767_SPI_ILI9341_2/767_SPI_ILI9341_2.ioc), and [screen GPIO definitions](767_SPI_ILI9341_2/Core/Inc/main.h): conflict check.
- [Lab 8 instructions](docs/2026-Lab-08-LCD.md), Screen 1 and Fig 1.1: temperature/humidity display requirement.
- [Adafruit AM2320 pinouts](https://learn.adafruit.com/adafruit-am2320-temperature-humidity-i2c-sensor/pinouts): sensor pins, supply, and pull-ups.
- [Adafruit AM2320 wiring](https://learn.adafruit.com/adafruit-am2320-temperature-humidity-i2c-sensor/arduino-usage): I2C connections and fixed address.
