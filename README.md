# PIC16F18325_WS2812bLEDDriver

This driver is based on an overclocked PIC16F18325 which runs at 64 MHz.
The watchdog timer can be disabled by commenting out the AUTO_RESET define.
It was used for the reason that after a reset of the master sometimes the LEDs show a different color than was transmitted.

The signal is transmitted in realtime every time a byte arrives on the SPI bus.
The SPI bus should be operated with 750 KHz. It can work also with other frequencies,
it should only be paid attention to the fact that the pause times of the LEDs are kept.
