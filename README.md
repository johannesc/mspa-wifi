# mspa-wifi

This project aims to add WiFi support to my M-Spa Aurora Urban U-AU062.

# Initial findings

## Hardware

The control board inside the control box:
![image](./images/control-box-pcb.jpg)

The microcontroller is a PIC16F723A:

![image](./images/control-box-pcb-pic16f723a.jpg)

The connector to the remote control can be seen in the upper right corner. The pinout:

| Color | Description                        |
|-------|------------------------------------|
| White | GND                                |
| Green | Rx (remote control to control box) |
| Black | Tx (control box to remote control) |
| Red   | +5v                                |


## Protocol

UART Settings 9600 bps 8N1. 4 byte packets.

See `scripts/mspa_decoder.py` for example decoding of the protocol.

Captured data:

![Captured startup communication](./images/saleae-startup.png)

### Example Packets

#### From Control Box:
| Data | Description                 |
|------|-----------------------------|
|0xA5| Packet start                  |
|0x06| Temperature reading           |
|0x1C| Temp = 0x1C/2 = 14.0 degrees  |
|0xC7| Checksum = 0xA5 + 0x06 + 0X1C)|

More examples:

    0xA5 0x08 0x00 0xAD
    0xA5 0x0B 0x86 0x36

    Flow On?
    A5 08 01 AE

    A5 0B 86 36

#### From Remote

    Timestamp: Data
    0.30: A5 03 00 A8
    1.81: A5 0D 00 B2
    3.31: A5 0E 00 B3
    4.81: A5 15 00 BA
    4.91: A5 01 00 A6
    6.31: A5 02 00 A7

    Press - one time (temp at 40 degrees)
    110.67: A5 16 00 BB
    110.67: A5 04 28 D1

    Press - until 35 degrees:
    173.09: A5 04 23 CC

    Heater On:
    A5 02 01 A8
    A5 16 00 BB

    Filter On:
    A5 02 01 A8
    A5 16 00 BB
