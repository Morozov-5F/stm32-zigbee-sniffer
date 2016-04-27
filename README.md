# stm32-zigbee-sniffer

A ZigBee sniffer based on STM32F4Discovery board and OLIMEX MOD-MRF24J40 radio module

In this repo you can find a ibrary to work with MRF24J40 module. This library is designed only for 
sniffing purposes and I have no guaranties that it will work in other conditions. Also Full stack of HAL drivers
and data sheets are included in this repo. Why? I really don't know, I thought it would be simplier for me, but, actually,
it's not. Really sorry for all that HAL drivers included.

### Indication 

Sniffer itself uses kind of "standart-issue" chineese LCD display, based on HD44780. I used library from 
[this source](https://petoknm.wordpress.com/2015/01/04/serial-lcd-driver-for-stm32/), because I just thought 
it will be more stable than my own library, which I also have. Because this LCD displays only 80 bytes (and, if I want to make
those bytes human-readable, I have to output them in HEX, which takes half, so I've got 40 bytes), and packet length could be 
127 bytes, I decided to show only kindof important information - Frame Length, RSSI, and so on, without showing the whole packet.
But via serial port more info can be uploaded to PC or other device (this is implemented only via debug sequence).

### Acronyms used in LCD

| Acronym        | Defenition                                        |
| -------------- | ------------------------------------------------- |
| 'FL'           | Frame Length                                      |
| 'FT'           | Frame Type                                        |
| 'DAM' or 'SAM' | Address Mode, Destination or Source, respectively |
| 'SN'           | Sequence number                                   |
| 'FCS'          | Frame Control Sequence                            |
| 'RSSI'         | Received Signal Strength Indicator                |
