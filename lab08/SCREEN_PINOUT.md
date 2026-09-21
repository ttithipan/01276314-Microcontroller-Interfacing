# NUCLEO-F767ZI wiring for the ILI9341 screen

This wiring matches the existing `767_SPI_ILI9341_2` project, whose CubeMX device is **STM32F767ZITx**. Board connector positions were checked against **UM1974 Rev 11, Table 19, pages 61-65**. No firmware pin changes are needed for this assignment.

`CN9-26` means **connector CN9, physical header pin 26**. `D62` is the board signal name; `PF7` is the MCU GPIO name (port F, bit 7). MCU names below are GPIO identifiers, not the chip's numbered LQFP package leads. Connect jumper wires to the board headers.

## LCD display

| Screen terminal | MCU pin | Board header | Board signal | Firmware function |
| --- | --- | --- | --- | --- |
| SCK / CLK | PF7 | **CN9-26** | D62 | SPI5_SCK, AF5 |
| SDI / MOSI / SDA | PF9 | **CN9-28** | D63 | SPI5_MOSI, AF5; MCU to LCD |
| SDO / MISO | PF8 | **CN9-24** | D61 | SPI5_MISO, AF5; LCD to MCU; optional for this display driver |
| CS / LCD_CS | PC8 | **CN8-2** | D43 | GPIO output, `CS`; active low |
| DC / D-C / RS | PC9 | **CN8-4** | D44 | GPIO output, `DC`; command/data select |
| RESET / RST | PC10 | **CN8-6** | D45 | GPIO output, `RST`; LCD reset |

The driver selects `&hspi5`. The project configures SPI5 as master, 8-bit, MSB first, clock idle low and first-edge sampling (mode 0), with software chip select. The display driver only transmits data, so its SDO/MISO wire can be omitted for the supplied drawing functions even though PF8 is configured in the project. A terminal called SDA on this SPI screen is MOSI, not an I2C connection.

Connect screen RESET to **PC10**, not the board's NRST/RESET header: the supplied driver controls the LCD reset independently.

## Touch interface

The included touch driver uses **software SPI on separate GPIOs**. Keep these wires separate from LCD SPI5 when using the existing code.

| Screen terminal | MCU pin | Board header | Board signal | Firmware function |
| --- | --- | --- | --- | --- |
| T_IRQ / PENIRQ | PE2 | **CN9-14** | D56 | Input, `T_IRQ`; low when pressed |
| T_CLK | PE3 | **CN9-22** | D60 | Output, `T_CLK`; touch clock |
| T_DO / T_MISO | PE4 | **CN9-16** | D57 | Input, `T_MISO`; touch controller to MCU |
| T_DIN / T_MOSI | PE5 | **CN9-18** | D58 | Output, `T_MOSI`; MCU to touch controller |
| T_CS | PE6 | **CN9-20** | D59 | Output, `T_CS`; active low |

Touch T_DO/MISO is required for reading coordinates; it is distinct from the optional LCD SDO/MISO. These five wires apply to a module exposing the SPI touch interface expected by the supplied driver. The actual touch-controller model is not established by the ILI9341 display-controller name alone.

PE2 also appears on **CN10-25 (D31)**. UM1974 Table 19 footnote 3 says these are the same MCU pin and only one function may be used at a time. Use CN9-14 here.

## Power and backlight

| Screen terminal | Board connection | Notes |
| --- | --- | --- |
| GND | **CN8-11** or **CN8-13** | Common ground; CN9-12 or CN9-23 also provides GND. |
| VCC | **CN8-7 (+3.3 V)**, if the module accepts a 3.3 V supply | Confirm the screen module's supply specification; the driver does not identify its regulator or voltage requirements. |
| VCC, only if the module specifically requires 5 V input | **CN8-9 (+5 V)** | Alternative supply, not an additional connection. This does not authorize 5 V logic signals. |
| LED / BL | Module-dependent | No backlight-control GPIO is defined in this project. Connect to 3.3 V only if the module specifies a 3.3 V backlight input with suitable current limiting/driver. A bare LED backlight needs appropriate current limiting or a driver. |

Use 3.3 V logic for the screen signals. Verify module VCC and LED/BL requirements before powering it; UM1974 describes the Nucleo board, not the screen module. Do not connect both power rails to VCC.

## Optional ST morpho header equivalents

The CN8/CN9 connections above are sufficient. If using the ST morpho headers instead, UM1974 **Table 21, page 70** gives these equivalent connections. Use one header connection per signal.

| Signal | MCU pin | ST morpho header |
| --- | --- | --- |
| LCD SCK | PF7 | CN11-11 |
| LCD MISO | PF8 | CN11-54 |
| LCD MOSI | PF9 | CN11-56 |
| LCD CS | PC8 | CN12-2 |
| LCD DC | PC9 | CN12-1 |
| LCD RESET | PC10 | CN11-1 |
| Touch IRQ | PE2 | CN11-46 |
| Touch CLK | PE3 | CN11-47 |
| Touch MISO | PE4 | CN11-48 |
| Touch MOSI | PE5 | CN11-50 |
| Touch CS | PE6 | CN11-62 |

## Configuration and wiring notes

- Locate each connector's pin-1 marking before counting positions. Odd and even pins form separate rows; adjacent pins along one row increment by two.
- The SAI/SDMMC function names in UM1974 identify alternate uses of these MCU pins. This project configures them for SPI5 or GPIO instead; do not enable competing SAI/SDMMC functions on the same pins.
- Keep PC10 reserved for LCD reset if adding buttons or other lab peripherals.
- The touch inputs are currently configured with no internal pull resistors. Confirm the module provides the required idle-high IRQ level; if not, add/configure a suitable pull-up to 3.3 V.
- A module's microSD socket is not covered by these drivers or this pin list.
- This is a source- and manual-verified wiring assignment, not a hardware-tested result. Screen connector order, supply requirements, and backlight circuit still depend on the actual module.

## Sources

- [UM1974 board manual](docs/um1974-stm32-nucleo144-boards-mb1137-stmicroelectronics.pdf): Table 19 pp. 61-62 for CN8/CN9, footnote 3 p. 65 for PE2 duplication, Table 21 p. 70 for ST morpho equivalents.
- [CubeMX project](767_SPI_ILI9341_2/767_SPI_ILI9341_2.ioc): MCU selection and pin assignments.
- [main.h](767_SPI_ILI9341_2/Core/Inc/main.h): LCD and touch GPIO definitions.
- [spi.c](767_SPI_ILI9341_2/Core/Src/spi.c): SPI5 settings and PF7/PF8/PF9 alternate functions.
- [gpio.c](767_SPI_ILI9341_2/Core/Src/gpio.c): LCD/touch input and output configuration.
- [ILI9341_STM32_Driver.h](767_SPI_ILI9341_2/Drivers/ILI9341/ILI9341_STM32_Driver.h) and [driver implementation](767_SPI_ILI9341_2/Drivers/ILI9341/ILI9341_STM32_Driver.c): LCD SPI handle, control pins, and transmit-only drawing operations.
- [ILI9341_Touchscreen.h](767_SPI_ILI9341_2/Drivers/ILI9341/ILI9341_Touchscreen.h) and [touch implementation](767_SPI_ILI9341_2/Drivers/ILI9341/ILI9341_Touchscreen.c): separate software-SPI touch interface and active-low IRQ handling.
