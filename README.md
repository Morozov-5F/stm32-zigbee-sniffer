# STM32-zigbee-sniffer

A ZigBee sniffer based on STM32F4Discovery board and OLIMEX MOD-MRF24J40 radio module

In this repo you can find a library to work with MRF24J40 module. This library is designed only for sniffing purposes and I have no guaranties that it will work in other conditions.

**Note:** You can find documentation in [docs folder](https://github.com/Morozov-5F/stm32-zigbee-sniffer/tree/master/docs) of this repository

### Indication

LCD Display for this sniffer - WH2004A LCD display with character generator - has to be connected via I2C interface. Library for given LCD was borrowed from [this source](https://petoknm.wordpress.com/2015/01/04/
serial-lcd-driver-for-stm32/). Data shown on display is very small - that's because
of lack of space on display. I had to use acronyms to show something more than just
frame type.

### Pinout
Pinout for STM32F4:

![STM32F4 Pinout](http://i.imgur.com/ZF8dYKp.png)

Pinout for MRF24J40 can be found in [OLIMEX Data Sheet](https://github.com/Morozov-5F/stm32-zigbee-sniffer/blob/master/docs/MRF24J40_user_manual.pdf).

Full data sheet for project can be found [here](https://github.com/Morozov-5F/stm32-zigbee-sniffer/blob/master/docs/stm32-zigbee-sniffer.pdf)

### Acronyms used in LCD

| Acronym        | Definition                                           |
| -------------- | ---------------------------------------------------- |
|  FL            | Frame Length                                         |
|  FT            | Frame Type                                           |
|  DAM or SAM    | Addressing Mode, Destination or Source, respectively |
|  SN            | Sequence number                                      |
|  FCS           | Frame Control Sequence                               |
|  RSSI          | Received Signal Strength Indicator                   |
